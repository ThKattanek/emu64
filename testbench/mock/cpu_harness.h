// Test harness for the MOS6510 CPU chip.
//
// Sets up a flat 64 KB RAM, wires all 256 read/write page-dispatch entries to
// that RAM, and provides the external signals (RESET, RDY, Breakpoints, …)
// required by MOS6510::OneZyklus().
//
// Usage:
//   CpuTestHarness h;
//   h.write(0x0400, {0xA9, 0x42});   // LDA #$42
//   h.do_reset();
//   h.run_cycles(2);
//   assert(h.get_ac() == 0x42);

#pragma once

#include <cstring>
#include <functional>
#include <cstdint>

// Chip headers live one level up in src/
#include "mos6510_class.h"
#include "structs.h"

class CpuTestHarness
{
public:
    // ── Public RAM (freely writable by tests) ────────────────────────────
    uint8_t ram[65536];

    // ── External signals required by MOS6510 ────────────────────────────
    bool     reset_wire   = false;   // Active-low pin: false = reset asserted
    bool     rdy          = true;
    bool     reset_ready  = false;

    // Debugger arrays (all zeroed – break-checking is a no-op in tests)
    uint16_t breakpoints[0x10000];
    uint16_t break_values[16];
    uint16_t break_status = 0;
    uint16_t pc_history[256];
    uint8_t  history_pointer = 0;

    // ── Page-dispatch tables ─────────────────────────────────────────────
    std::function<uint8_t(uint16_t)>            read_tbl[256];
    std::function<void(uint16_t, uint8_t)>      write_tbl[256];

    // ── The CPU instance ─────────────────────────────────────────────────
    MOS6510 cpu;

    // ── Constructor – wires everything up but does NOT reset the CPU ─────
    CpuTestHarness()
    {
        memset(ram,          0, sizeof(ram));
        memset(breakpoints,  0, sizeof(breakpoints));
        memset(break_values, 0, sizeof(break_values));
        memset(pc_history,   0, sizeof(pc_history));

        for (int i = 0; i < 256; i++)
        {
            read_tbl[i]  = [this](uint16_t a) -> uint8_t          { return ram[a]; };
            write_tbl[i] = [this](uint16_t a, uint8_t v)          { ram[a] = v; };
        }

        cpu.ReadProcTbl        = read_tbl;
        cpu.WriteProcTbl       = write_tbl;
        cpu.RDY                = &rdy;
        cpu.RESET              = &reset_wire;
        cpu.ResetReady         = &reset_ready;
        cpu.ResetReadyAdr      = 0x0000;
        cpu.BreakStatus        = &break_status;
        cpu.Breakpoints        = breakpoints;
        cpu.BreakWerte         = break_values;
        cpu.History            = pc_history;
        cpu.HistoryPointer     = &history_pointer;
        cpu.EnableExtInterrupts = false;
        cpu.WRITE_FF00         = false;
    }

    // ── Helpers ──────────────────────────────────────────────────────────

    /// Set the reset vector ($FFFC/$FFFD)
    void set_reset_vector(uint16_t addr)
    {
        ram[0xFFFC] = static_cast<uint8_t>(addr);
        ram[0xFFFD] = static_cast<uint8_t>(addr >> 8);
    }

    /// Set the IRQ vector ($FFFE/$FFFF)
    void set_irq_vector(uint16_t addr)
    {
        ram[0xFFFE] = static_cast<uint8_t>(addr);
        ram[0xFFFF] = static_cast<uint8_t>(addr >> 8);
    }

    /// Set the NMI vector ($FFFA/$FFFB)
    void set_nmi_vector(uint16_t addr)
    {
        ram[0xFFFA] = static_cast<uint8_t>(addr);
        ram[0xFFFB] = static_cast<uint8_t>(addr >> 8);
    }

    /// Write a sequence of bytes into RAM starting at `addr`
    void write(uint16_t addr, std::initializer_list<uint8_t> bytes)
    {
        for (uint8_t b : bytes)
            ram[addr++] = b;
    }

    /// Perform a full hardware reset:
    ///  1. Assert RESET (false)   → CPU enters wait state (3 cycles to stabilise)
    ///  2. Deassert RESET (true)  → rising edge triggers the 3-step reset sequence:
    ///       step 96 (read $FFFC), step 97 (read $FFFD), case-0 (opcode fetch)
    ///  After do_reset() the MCT is pointing at the first micro-code step of the
    ///  instruction at the reset vector – ready to be executed by run_cycles().
    void do_reset()
    {
        // Assert RESET – let the CPU see reset_wire = false so RESET_OLD becomes
        // false, avoiding a spurious rising edge if this harness is reused.
        reset_wire = false;
        for (int i = 0; i < 3; i++)
        {
            cpu.OneZyklus();
            cpu.Phi1();
        }

        // Rising edge: the 3-step reset sequence runs.
        //   Cycle 1: step 96 → reads $FFFC (PC lo)
        //   Cycle 2: step 97 → reads $FFFD (PC hi), then hits terminator
        //   Cycle 3: case 0  → opcode fetch; MCT = first step of first instruction
        reset_wire = true;
        for (int i = 0; i < 3; i++)
        {
            cpu.OneZyklus();
            cpu.Phi1();
        }
    }

    /// Advance the CPU by exactly `n` cycles.
    /// Phi1() is called after each OneZyklus() to match the real C64 main loop
    /// (required for correct IRQ/NMI detection).
    void run_cycles(int n)
    {
        for (int i = 0; i < n; i++)
        {
            cpu.OneZyklus();
            cpu.Phi1();
        }
    }

    /// Run until PC matches `target` or `max_cycles` are exhausted.
    /// Returns true on success.
    bool run_until_pc(uint16_t target, uint64_t max_cycles = 1'000'000)
    {
        for (uint64_t i = 0; i < max_cycles; i++)
        {
            cpu.OneZyklus();
            cpu.Phi1();
            if (get_pc() == target)
                return true;
        }
        return false;
    }

    /// Run until the PC loops (branch-to-self) for `stable_cycles` consecutive
    /// cycles, then return the trapped PC.  Used by functional-test runners.
    uint16_t run_until_trap(uint64_t max_cycles = 100'000'000, int stable_cycles = 4)
    {
        uint16_t prev = 0xFFFF;
        int      same = 0;
        for (uint64_t i = 0; i < max_cycles; i++)
        {
            cpu.OneZyklus();
            cpu.Phi1();
            uint16_t pc = get_pc();
            if (pc == prev)
            {
                if (++same >= stable_cycles)
                    return pc;
            }
            else
            {
                prev = pc;
                same = 0;
            }
        }
        return get_pc();
    }

    // ── Register accessors ───────────────────────────────────────────────

    uint16_t get_pc()
    {
        REG_STRUCT r;
        r.reg_mask = REG_MASK_PC;
        cpu.GetRegister(&r);
        return r.pc;
    }

    uint8_t get_ac()
    {
        REG_STRUCT r;
        r.reg_mask = REG_MASK_ALL;
        cpu.GetRegister(&r);
        return r.ac;
    }

    uint8_t get_xr()
    {
        REG_STRUCT r;
        r.reg_mask = REG_MASK_ALL;
        cpu.GetRegister(&r);
        return r.xr;
    }

    uint8_t get_yr()
    {
        REG_STRUCT r;
        r.reg_mask = REG_MASK_ALL;
        cpu.GetRegister(&r);
        return r.yr;
    }

    uint8_t get_sp()
    {
        REG_STRUCT r;
        r.reg_mask = REG_MASK_ALL;
        cpu.GetRegister(&r);
        return r.sp;
    }

    uint8_t get_sr()
    {
        REG_STRUCT r;
        r.reg_mask = REG_MASK_ALL;
        cpu.GetRegister(&r);
        return r.sr;
    }

    // Convenience flag accessors (from the Status Register)
    bool flag_N() { return (get_sr() & 0x80) != 0; }
    bool flag_V() { return (get_sr() & 0x40) != 0; }
    bool flag_I() { return (get_sr() & 0x04) != 0; }
    bool flag_Z() { return (get_sr() & 0x02) != 0; }
    bool flag_C() { return (get_sr() & 0x01) != 0; }
};
