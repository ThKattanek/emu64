// Test harness for the MOS6526 CIA chip.
//
// Wires up both PORT objects, the reset signal, and the CPU-trigger callbacks
// with lightweight lambdas so the CIA can be exercised without Qt / SDL.
//
// Usage:
//   CiaTestHarness h;
//   h.cia.Reset();
//   // Write timer-A latch = 5, start one-shot
//   h.cia.WriteIO(0x04, 5); h.cia.WriteIO(0x05, 0);
//   h.cia.WriteIO(0x0D, 0x81);   // enable Timer-A IRQ
//   h.cia.WriteIO(0x0E, 0x19);   // start, one-shot, force-load
//   h.run_cycles(7);
//   assert(h.irq_count > 0);

#pragma once

#include <cstring>
#include <cstdint>
#include <functional>

#include "mos6526_class.h"
#include "mos6526_port.h"
#include "structs.h"

class CiaTestHarness
{
public:
    // ── Externally observable state ──────────────────────────────────────
    int irq_count = 0;   // incremented each time CIA fires CIA_IRQ / CIA_NMI
    int irq_clear_count = 0;

    // ── Hardware pins ────────────────────────────────────────────────────
    bool    reset_wire  = true;   // true = running (not in reset)
    bool    flag_pin    = true;   // FLAG pin (active-low); null-safe in CIA code
    uint8_t c64_iec     = 0xFF;
    uint8_t floppy_iec  = 0xFF;

    // ── PORT objects for PA / PB ─────────────────────────────────────────
    PORT port_a;
    PORT port_b;

    // ── The CIA instance ─────────────────────────────────────────────────
    MOS6526 cia;

    // ── Constructor (cia_nr = 0 → CIA1, 1 → CIA2) ────────────────────────
    explicit CiaTestHarness(uint8_t cia_nr = 0) : cia(cia_nr)
    {
        // External signal wires.  Start with RESET asserted (low) so that the
        // first rising edge triggers a proper hardware reset inside OneZyklus().
        reset_wire = false;

        cia.reset_wire     = &reset_wire;
        cia.pa             = &port_a;
        cia.pb             = &port_b;
        cia.flag_pin       = nullptr;  // not connected in basic tests
        cia.c64_iec_wire   = &c64_iec;
        cia.floppy_iec_wire = &floppy_iec;

        // CPU interrupt callbacks
        cia.CpuTriggerInterrupt = [this](int) { ++irq_count; };
        cia.CpuClearInterrupt   = [this](int) { ++irq_clear_count; };

        // VIC light-pen (only called from CIA1 PB write; no-op here)
        cia.VicTriggerLP = []() {};

        // Paddle-switch (only called from CIA1 PA/DDRA write; no-op here)
        cia.ChangePOTSwitch = []() {};

        // Perform a proper hardware reset: let the rising edge of reset_wire
        // call cia.Reset() internally, leaving the CIA in a clean state.
        do_reset();
    }

    // ── Helpers ──────────────────────────────────────────────────────────

    /// Assert then deassert RESET, matching what the C64 does on power-up.
    /// Advances one cycle with RESET low (so reset_wire_old becomes false),
    /// then one cycle with RESET high (rising edge → cia.Reset() called).
    void do_reset()
    {
        irq_count       = 0;
        irq_clear_count = 0;

        reset_wire = false;
        cia.OneZyklus(); // reset_wire_old = false

        reset_wire = true;
        cia.OneZyklus(); // rising edge → cia.Reset() called inside
    }

    /// Advance the CIA by `n` cycles
    void run_cycles(int n)
    {
        for (int i = 0; i < n; i++)
            cia.OneZyklus();
    }

    /// Read timer-A current value (16-bit)
    uint16_t timer_a()
    {
        return static_cast<uint16_t>(cia.ReadIO(0x04)) |
               static_cast<uint16_t>(cia.ReadIO(0x05) << 8);
    }

    /// Read timer-B current value (16-bit)
    uint16_t timer_b()
    {
        return static_cast<uint16_t>(cia.ReadIO(0x06)) |
               static_cast<uint16_t>(cia.ReadIO(0x07) << 8);
    }

    /// Read ICR (clears it, as on real hardware)
    uint8_t read_icr()
    {
        return cia.ReadIO(0x0D);
    }
};
