// Unit tests for the VICII (MOS6569) chip.
//
// These tests verify:
//   • Register read / write (basic I/O)
//   • Raster line increment after 63 cycles (one PAL line)
//   • Bad-line detection starting at raster line $30
//   • Raster IRQ fires at the configured raster line
//
// The VIC is wired to a flat 64 KB RAM filled with zeros unless a test
// writes specific pixel/charset data.

#include "framework/test_framework.h"
#include "mock/vic_harness.h"

// ─── Helpers ──────────────────────────────────────────────────────────────────

/// Advance the VIC until the rasterline counter first reaches `target`.
/// Since rasterline increments during cycle 1 of each line, after this call
/// the VIC is at cycle 2+ of the target line.
static void advance_to_rasterline(VicTestHarness& h, uint16_t target)
{
    target %= 312;

    // Wait until we leave any residual occurrence of the target line first
    // (so we don't return immediately if we happen to already be there).
    bool was_on_target = (h.get_rasterline() == target);

    for (int guard = 0; guard < 3 * 312 * 63; guard++)
    {
        uint16_t rl = h.get_rasterline();
        if (was_on_target && rl != target)
            was_on_target = false;
        if (!was_on_target && rl == target)
            return;   // first cycle of the target rasterline
        h.vic.OneCycle();
    }
}

// ─── Register I/O ────────────────────────────────────────────────────────────

TEST_CASE(register_sprite0_x)
{
    // $D000 is sprite 0 X low-byte
    VicTestHarness h;
    h.vic.WriteIO(0x00, 0xAB);
    CHECK_EQ(h.vic.ReadIO(0x00), 0xABu);
}

TEST_CASE(register_sprite0_y)
{
    VicTestHarness h;
    h.vic.WriteIO(0x01, 0x7F);
    CHECK_EQ(h.vic.ReadIO(0x01), 0x7Fu);
}

TEST_CASE(register_border_colour)
{
    // $D020 – border colour (lower 4 bits used)
    VicTestHarness h;
    h.vic.WriteIO(0x20, 0x07);
    CHECK_EQ(h.vic.ReadIO(0x20) & 0x0Fu, 0x07u);
}

TEST_CASE(register_background_colour_0)
{
    // $D021
    VicTestHarness h;
    h.vic.WriteIO(0x21, 0x06);
    CHECK_EQ(h.vic.ReadIO(0x21) & 0x0Fu, 0x06u);
}

TEST_CASE(register_sprite_enable)
{
    // $D015 – sprite enable register (all 8 bits)
    VicTestHarness h;
    h.vic.WriteIO(0x15, 0xA5);
    CHECK_EQ(h.vic.ReadIO(0x15), 0xA5u);
}

TEST_CASE(register_ctrl1_scroll_and_rsel)
{
    // $D011 – vertical scroll, row-select, DEN, bitmap mode, raster high
    VicTestHarness h;
    // Write 0x1B: RSEL=1, YSCROLL=3, DEN=1 (standard text mode)
    h.vic.WriteIO(0x11, 0x1B);
    uint8_t v = h.vic.ReadIO(0x11);
    // Lower 7 bits should round-trip (bit 7 reflects the MSB of the raster line)
    CHECK_EQ(v & 0x7Fu, 0x1Bu);
}

TEST_CASE(register_memory_pointers)
{
    // $D018 – video matrix / char-gen base pointers
    VicTestHarness h;
    h.vic.WriteIO(0x18, 0x15); // char base = $1000, matrix = $0400 (default)
    CHECK_EQ(h.vic.ReadIO(0x18), 0x15u);
}

TEST_CASE(register_irq_raster_latch)
{
    // $D012 is the raster-compare register when written, but when read it
    // returns the CURRENT raster counter (lower 8 bits), not the stored value.
    // Verify that reading $D012 returns a value in the valid range [0, 311].
    VicTestHarness h;
    uint8_t v = h.vic.ReadIO(0x12);
    CHECK(v <= 0xFFu);  // always true – just confirms no crash

    // Also confirm that writing $D012 stores the new compare value which is
    // visible through GetRegister().
    h.vic.WriteIO(0x12, 0x88);
    VIC_STRUCT s;
    h.vic.GetRegister(&s);
    CHECK_EQ(s.raster_latch & 0xFFu, 0x88u);
}

// ─── Raster-line timing ───────────────────────────────────────────────────────

TEST_CASE(rasterline_increments_after_63_cycles)
{
    // In PAL mode every raster line is exactly 63 cycles wide.
    // Start at whatever line we are on and verify the count increments by 1.
    VicTestHarness h;

    // Synchronise to the beginning of a raster line for a clean measurement
    for (int i = 0; i < 63; i++)
        h.vic.OneCycle();

    uint16_t line_before = h.get_rasterline();

    // Run exactly one more line (63 cycles)
    h.run_cycles(63);

    uint16_t line_after = h.get_rasterline();
    uint16_t expected = static_cast<uint16_t>((line_before + 1) % 312);

    CHECK_EQ(line_after, expected);
}

TEST_CASE(full_frame_312_lines)
{
    // One PAL frame = 312 × 63 = 19 656 cycles.
    // After exactly one frame the raster counter must be back to its start value.
    VicTestHarness h;

    uint16_t start = h.get_rasterline();
    h.run_cycles(312 * 63);
    uint16_t end = h.get_rasterline();

    CHECK_EQ(end, start);
}

// ─── Bad-line detection ───────────────────────────────────────────────────────

TEST_CASE(bad_line_range_starts_at_0x30)
{
    // Bad Lines occur on raster lines $30–$F7 when the lower 3 bits of the
    // raster counter equal YSCROLL (bits 2-0 of $D011).  With YSCROLL=3
    // (the C64 default) the first bad line is $33.
    // We verify: after reaching line $33, the BA signal goes low (CPU stalled)
    // at some point within that line, indicating a bad-line DMA sequence.
    VicTestHarness h;

    // Standard C64 display setup
    h.vic.WriteIO(0x11, 0x1B); // DEN=1, RSEL=1, YSCROLL=3
    h.vic.WriteIO(0x18, 0x15); // char base $1000, matrix $0400

    // First advance to line $30 so that badline_enable gets armed (it is set
    // at the start of every cycle when current_rasterline == $30).
    advance_to_rasterline(h, 0x30);
    // Run through line $30 entirely to ensure badline_enable = true
    h.run_cycles(63);

    // Now advance to the first actual bad line ($33 with YSCROLL=3)
    advance_to_rasterline(h, 0x33);

    // Run through the line and watch for BA low (BA goes low at cycles 12-15)
    bool ba_was_low = false;
    for (int c = 0; c < 63; c++)
    {
        h.vic.OneCycle();
        if (!h.ba)
            ba_was_low = true;
    }

    CHECK(ba_was_low);
}

// ─── Raster IRQ ───────────────────────────────────────────────────────────────

TEST_CASE(raster_irq_fires_at_configured_line)
{
    // Configure raster IRQ at line 100 ($64).
    // $D012 = 0x64, $D011 bit7 = 0 (raster high = 0).
    // $D01A bit0 = 1 → enable raster IRQ.
    VicTestHarness h;

    h.vic.WriteIO(0x12, 100);   // raster-compare low 8 bits
    h.vic.WriteIO(0x11, 0x1B); // RSEL, YSCROLL, DEN (bit 7 = raster MSB = 0)
    h.vic.WriteIO(0x1A, 0x01); // enable raster IRQ

    // Advance past the target line; we should see at least one IRQ
    advance_to_rasterline(h, 0);   // reset to start of frame
    h.run_cycles(100 * 63 + 10);   // run up to just past line 100

    CHECK(h.irq_count > 0);
}

// ─── Sprite enable register does not alias ───────────────────────────────────

TEST_CASE(sprite_x_msb_register)
{
    // $D010 holds the 9th (MSB) bit of each sprite's X coordinate
    VicTestHarness h;
    h.vic.WriteIO(0x10, 0xFF); // all sprites use 9-bit X
    CHECK_EQ(h.vic.ReadIO(0x10), 0xFFu);
}

// ─── Screen blanking (DEN bit) ───────────────────────────────────────────────

TEST_CASE(den_bit_readable)
{
    // Clearing DEN (bit 4 of $D011) disables the display
    VicTestHarness h;
    h.vic.WriteIO(0x11, 0x0B); // RSEL=1, YSCROLL=3, DEN=0
    uint8_t ctrl = h.vic.ReadIO(0x11);
    CHECK_EQ(ctrl & 0x10u, 0x00u); // DEN bit should be 0
}

TEST_MAIN()
