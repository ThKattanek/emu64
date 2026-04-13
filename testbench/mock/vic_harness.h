// Test harness for the VICII (MOS6569) chip.
//
// Provides a flat 64 KB address space, colour RAM, and stub callbacks so the
// VIC can be exercised without Qt / SDL.
//
// Usage:
//   VicTestHarness h;
//   h.vic.OneCycle();
//   // After 63 cycles (one PAL raster line) the raster counter increments
//   uint16_t line = h.get_rasterline();

#pragma once

#include <cstring>
#include <cstdint>
#include <functional>

#include "mos6569_class.h"
#include "structs.h"

class VicTestHarness
{
public:
    // ── Address space ────────────────────────────────────────────────────
    uint8_t  ram[65536];
    uint8_t  color_ram[1024];

    // ── External signals ─────────────────────────────────────────────────
    bool    ba          = true;
    uint8_t cia2_port_a = 0x03;  // bits 0-1 inverted = VIC bank 0 ($0000)
    uint8_t cia1_port_b = 0xFF;

    // ── Debugger arrays (all zero = no breakpoints) ───────────────────────
    uint16_t breakpoints[0x10000];
    uint16_t break_status = 0;
    uint16_t break_values[16];

    // ── Page-dispatch table (all pages map to flat RAM) ───────────────────
    std::function<uint8_t(uint16_t)> read_tbl[256];

    // ── Observable state ──────────────────────────────────────────────────
    int refresh_count = 0;   // incremented each time RefreshProc is called
    int irq_count     = 0;

    // ── The VIC instance ──────────────────────────────────────────────────
    VICII vic;

    VicTestHarness()
    {
        memset(ram,          0, sizeof(ram));
        memset(color_ram,    0, sizeof(color_ram));
        memset(breakpoints,  0, sizeof(breakpoints));
        memset(break_values, 0, sizeof(break_values));

        for (int i = 0; i < 256; i++)
            read_tbl[i] = [this](uint16_t a) -> uint8_t { return ram[a]; };

        vic.ReadProcTbl  = read_tbl;
        vic.RefreshProc  = [this](uint8_t*) { ++refresh_count; };
        vic.CpuTriggerInterrupt = [this](int) { ++irq_count; };
        vic.CpuClearInterrupt   = [](int) {};

        vic.ba            = &ba;
        vic.cia2_port_a   = &cia2_port_a;
        vic.cia1_port_b   = &cia1_port_b;
        vic.color_ram     = color_ram;
        vic.breakpoints   = breakpoints;
        vic.break_status  = &break_status;
        vic.break_values  = break_values;

        // Select PAL mode (system 0)
        vic.SetVicType(0);
    }

    // ── Helpers ───────────────────────────────────────────────────────────

    /// Advance the VIC by `n` cycles
    void run_cycles(int n)
    {
        for (int i = 0; i < n; i++)
            vic.OneCycle();
    }

    /// Read the current raster line from the VIC struct
    uint16_t get_rasterline()
    {
        VIC_STRUCT s;
        vic.GetRegister(&s);
        return s.current_rasterline;
    }

    /// Read the current cycle-within-line from the VIC struct
    uint16_t get_cycle()
    {
        VIC_STRUCT s;
        vic.GetRegister(&s);
        return s.current_cycle;
    }
};
