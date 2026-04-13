// Unit tests for the MOS6526 CIA chip.
//
// Each TEST_CASE creates a fresh CiaTestHarness, configures the CIA, runs
// the required number of cycles, and checks observable state (timer values,
// IRQ count, ICR contents).

#include "framework/test_framework.h"
#include "mock/cia_harness.h"

// ─── Timer A decrements ───────────────────────────────────────────────────────

TEST_CASE(timer_a_counts_down)
{
    // Load latch = 100, start continuous counting (phi2)
    // CRA = 0x11 (start=1, force-load=1, one-shot=0)
    CiaTestHarness h;
    h.cia.WriteIO(0x04, 100); // Timer A latch lo
    h.cia.WriteIO(0x05, 0);   // Timer A latch hi
    h.cia.WriteIO(0x0E, 0x11); // start + force-load

    // The force-load startup sequence requires 3 cycles before counting begins
    // (STOP → LOAD_THEN_WAIT_THEN_COUNT → WAIT_THEN_COUNT → COUNT).
    // Run 5 cycles to reach steady-state counting.
    h.run_cycles(5);
    uint16_t t1 = h.timer_a();

    h.run_cycles(10);
    uint16_t t2 = h.timer_a();

    // Timer must have decreased by exactly 10 (one per cycle in steady state)
    CHECK(t1 > t2);
    CHECK_EQ(static_cast<uint16_t>(t1 - t2), 10u);
}

// ─── Timer A underflow in one-shot mode ───────────────────────────────────────

TEST_CASE(timer_a_oneshot_underflow)
{
    // Latch = 3, one-shot: timer counts 3 → 2 → 1 → underflow → stops, IRQ
    // Startup delay = 3 cycles (STOP→LOAD_THEN_WAIT_THEN_COUNT→WAIT_THEN_COUNT→COUNT).
    // Underflow occurs at cycle 3+3 = 6 from WriteIO(0x0E) onwards (run 8 to be safe).
    // CRA = 0x19 (start=1, one-shot=1, force-load=1)
    CiaTestHarness h;

    // Enable Timer-A interrupt in ICR mask
    h.cia.WriteIO(0x0D, 0x81); // set bit 7 (set) + bit 0 (Timer A)

    h.cia.WriteIO(0x04, 3);    // Timer A latch lo = 3
    h.cia.WriteIO(0x05, 0);    // Timer A latch hi = 0
    h.cia.WriteIO(0x0E, 0x19); // start + one-shot + force-load

    h.run_cycles(8);

    CHECK(h.irq_count > 0);
}

// ─── Timer A one-shot stops after underflow ───────────────────────────────────

TEST_CASE(timer_a_oneshot_stops)
{
    CiaTestHarness h;
    h.cia.WriteIO(0x0D, 0x81); // enable Timer A IRQ
    h.cia.WriteIO(0x04, 2);
    h.cia.WriteIO(0x05, 0);
    h.cia.WriteIO(0x0E, 0x19); // start, one-shot, force-load

    h.run_cycles(10); // well past underflow

    // Only one IRQ should have fired (one-shot mode)
    CHECK_EQ(h.irq_count, 1);
}

// ─── Timer A continuous mode fires repeatedly ─────────────────────────────────

TEST_CASE(timer_a_continuous_fires_multiple)
{
    // Latch = 2, continuous: underflow every 3 cycles (load→count→count→underflow)
    CiaTestHarness h;
    h.cia.WriteIO(0x0D, 0x81); // enable Timer A IRQ
    h.cia.WriteIO(0x04, 2);
    h.cia.WriteIO(0x05, 0);
    h.cia.WriteIO(0x0E, 0x11); // start, continuous, force-load

    h.run_cycles(20);

    // Must have fired more than once
    CHECK(h.irq_count > 1);
}

// ─── ICR is cleared by reading ────────────────────────────────────────────────

TEST_CASE(icr_cleared_after_read)
{
    CiaTestHarness h;
    h.cia.WriteIO(0x0D, 0x81); // enable Timer A IRQ
    h.cia.WriteIO(0x04, 1);
    h.cia.WriteIO(0x05, 0);
    h.cia.WriteIO(0x0E, 0x19); // one-shot, force-load, start

    h.run_cycles(5); // let timer underflow

    uint8_t icr1 = h.read_icr();   // should have bit 0 and bit 7 set
    uint8_t icr2 = h.read_icr();   // should be 0 after first read

    CHECK((icr1 & 0x81) == 0x81u); // Timer A + IRQ bits
    CHECK_EQ(icr2, 0x00u);
}

// ─── Timer B decrements ───────────────────────────────────────────────────────

TEST_CASE(timer_b_counts_down)
{
    // CRB = 0x11 (start + force-load, continuous, count phi2)
    CiaTestHarness h;
    h.cia.WriteIO(0x06, 50); // Timer B latch lo
    h.cia.WriteIO(0x07, 0);  // Timer B latch hi
    h.cia.WriteIO(0x0F, 0x11); // start + force-load

    // Run past startup delay (3 cycles) to reach steady-state counting
    h.run_cycles(5);
    uint16_t t1 = h.timer_b();
    h.run_cycles(10);
    uint16_t t2 = h.timer_b();

    CHECK(t1 > t2);
    CHECK_EQ(static_cast<uint16_t>(t1 - t2), 10u);
}

// ─── Timer B underflow generates IRQ ─────────────────────────────────────────

TEST_CASE(timer_b_oneshot_underflow)
{
    // Enable Timer B IRQ (bit 1 in ICR mask)
    CiaTestHarness h;
    h.cia.WriteIO(0x0D, 0x82); // set bit 7 + bit 1 (Timer B)

    h.cia.WriteIO(0x06, 3);
    h.cia.WriteIO(0x07, 0);
    h.cia.WriteIO(0x0F, 0x19); // start + one-shot + force-load

    h.run_cycles(6);

    CHECK(h.irq_count > 0);
}

// ─── Timer A latch reload after underflow ────────────────────────────────────

TEST_CASE(timer_a_latch_reload)
{
    // Latch = 10, continuous.  After underflow the timer should reload to 10.
    CiaTestHarness h;
    h.cia.WriteIO(0x04, 10);
    h.cia.WriteIO(0x05, 0);
    h.cia.WriteIO(0x0E, 0x11); // start, continuous, force-load

    // Run for one full underflow period + some cycles into the next
    h.run_cycles(11 + 3);

    uint16_t t = h.timer_a();
    // Timer should be somewhere in the range [1, 10] (reloaded, partially counted)
    CHECK(t > 0u);
    CHECK(t <= 10u);
}

// ─── ICR mask disabled – no IRQ fired ────────────────────────────────────────

TEST_CASE(timer_a_no_irq_when_masked)
{
    // Do NOT write to ICR mask → interrupt is disabled
    CiaTestHarness h;
    h.cia.WriteIO(0x04, 2);
    h.cia.WriteIO(0x05, 0);
    h.cia.WriteIO(0x0E, 0x19); // start, one-shot, force-load

    h.run_cycles(10);

    // ICR bit 0 may be set (timer underflowed), but IRQ callback should NOT
    // have fired because the mask bit was never enabled
    CHECK_EQ(h.irq_count, 0);
}

// ─── TOD register write / read ────────────────────────────────────────────────

TEST_CASE(tod_register_write_read)
{
    // The standard write sequence is: hours (stops TOD), minutes, seconds,
    // tenths (starts TOD).  The standard read sequence is: hours (latches
    // all), minutes, seconds, tenths (unlatches).
    CiaTestHarness h;

    h.cia.WriteIO(0x0B, 0x01);  // hours = 1 (BCD, stops TOD)
    h.cia.WriteIO(0x0A, 0x30);  // minutes = 30 BCD
    h.cia.WriteIO(0x09, 0x45);  // seconds = 45 BCD
    h.cia.WriteIO(0x08, 0x05);  // tenths = 5 BCD (starts TOD)

    // Reading hours latches the snapshot; reading tenths releases the latch.
    uint8_t hr = h.cia.ReadIO(0x0B);
    uint8_t mi = h.cia.ReadIO(0x0A);
    uint8_t se = h.cia.ReadIO(0x09);
    uint8_t te = h.cia.ReadIO(0x08);

    CHECK_EQ(hr & 0x1Fu, 0x01u);  // hours (mask AM/PM and unused bits)
    CHECK_EQ(mi, 0x30u);
    CHECK_EQ(se, 0x45u);
    CHECK_EQ(te, 0x05u);
}

// ─── Timer A 16-bit latch ────────────────────────────────────────────────────

TEST_CASE(timer_a_16bit_latch)
{
    // Latch = 0x0302
    CiaTestHarness h;
    h.cia.WriteIO(0x04, 0x02);  // lo
    h.cia.WriteIO(0x05, 0x03);  // hi
    // Timer is stopped; writing hi while stopped also loads the counter
    uint16_t t = h.timer_a();
    CHECK_EQ(t, 0x0302u);
}

TEST_MAIN()
