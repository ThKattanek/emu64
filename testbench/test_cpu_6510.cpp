// Unit tests for the MOS6510 CPU emulation.
//
// Each TEST_CASE is fully self-contained: it creates a fresh CpuTestHarness,
// writes a small 6510 machine-code snippet into RAM, performs a reset, runs
// the required number of cycles, and asserts on register/flag state.
//
// Cycle counts are documented in comments but are not enforced here – that
// would be the job of a separate timing-accuracy test suite.  These tests
// verify functional correctness first.

#include "framework/test_framework.h"
#include "mock/cpu_harness.h"

// ─── Helper: construct a 3-byte JMP $XXXX (opcode 4C lo hi) ──────────────────
static inline void jmp_self(CpuTestHarness& h, uint16_t addr)
{
    h.ram[addr]     = 0x4C;          // JMP abs
    h.ram[addr + 1] = static_cast<uint8_t>(addr);
    h.ram[addr + 2] = static_cast<uint8_t>(addr >> 8);
}

// ─── LDA immediate ───────────────────────────────────────────────────────────

TEST_CASE(lda_immediate_nonzero)
{
    // $0400: A9 A5  LDA #$A5  (2 cycles)
    // $0402: 4C 02 04  JMP $0402
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0xA5});
    jmp_self(h, 0x0402);
    h.do_reset();
    h.run_cycles(2);   // LDA #$A5

    CHECK_EQ(h.get_ac(), 0xA5u);
    CHECK(h.flag_N());   // bit 7 set
    CHECK(!h.flag_Z());
}

TEST_CASE(lda_immediate_zero)
{
    // $0400: A9 00  LDA #$00
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x00});
    jmp_self(h, 0x0402);
    h.do_reset();
    h.run_cycles(2);

    CHECK_EQ(h.get_ac(), 0x00u);
    CHECK(h.flag_Z());
    CHECK(!h.flag_N());
}

// ─── STA / LDA round-trip ────────────────────────────────────────────────────

TEST_CASE(sta_lda_roundtrip)
{
    // $0400: A9 7E  LDA #$7E        (2 cycles)
    // $0402: 85 10  STA $10         (3 cycles, zero-page)
    // $0404: A9 00  LDA #$00        (2 cycles) – clear A
    // $0406: A5 10  LDA $10         (3 cycles, zero-page)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x7E,   // LDA #$7E
                     0x85, 0x10,   // STA $10
                     0xA9, 0x00,   // LDA #$00
                     0xA5, 0x10}); // LDA $10
    jmp_self(h, 0x0408);
    h.do_reset();
    h.run_cycles(2 + 3 + 2 + 3);

    CHECK_EQ(h.get_ac(), 0x7Eu);
    CHECK(!h.flag_Z());
    CHECK(!h.flag_N());
}

// ─── ADC – no carry ───────────────────────────────────────────────────────────

TEST_CASE(adc_no_carry)
{
    // $0400: A9 10  LDA #$10  (2)
    // $0402: 18     CLC       (2)
    // $0403: 69 20  ADC #$20  (2)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x10, 0x18, 0x69, 0x20});
    jmp_self(h, 0x0405);
    h.do_reset();
    h.run_cycles(2 + 2 + 2);

    CHECK_EQ(h.get_ac(), 0x30u);
    CHECK(!h.flag_C());
    CHECK(!h.flag_N());
    CHECK(!h.flag_Z());
    CHECK(!h.flag_V());
}

// ─── ADC – result wraps and sets carry ───────────────────────────────────────

TEST_CASE(adc_carry_out)
{
    // $0400: A9 FE  LDA #$FE  (2)
    // $0402: 18     CLC       (2)
    // $0403: 69 03  ADC #$03  (2)  → $01, carry set
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0xFE, 0x18, 0x69, 0x03});
    jmp_self(h, 0x0405);
    h.do_reset();
    h.run_cycles(6);

    CHECK_EQ(h.get_ac(), 0x01u);
    CHECK(h.flag_C());
    CHECK(!h.flag_N());
}

// ─── ADC – signed overflow ────────────────────────────────────────────────────

TEST_CASE(adc_signed_overflow)
{
    // 0x50 + 0x50 = 0xA0; positive + positive = negative → V set
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x50,   // LDA #$50
                     0x18,          // CLC
                     0x69, 0x50}); // ADC #$50
    jmp_self(h, 0x0405);
    h.do_reset();
    h.run_cycles(6);

    CHECK_EQ(h.get_ac(), 0xA0u);
    CHECK(h.flag_V());
    CHECK(h.flag_N());
    CHECK(!h.flag_C());
}

// ─── SBC ─────────────────────────────────────────────────────────────────────

TEST_CASE(sbc_basic)
{
    // $0400: A9 09  LDA #$09  (2)
    // $0402: 38     SEC       (2)
    // $0403: E9 03  SBC #$03  (2)  → $06, borrow clear
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x09, 0x38, 0xE9, 0x03});
    jmp_self(h, 0x0405);
    h.do_reset();
    h.run_cycles(6);

    CHECK_EQ(h.get_ac(), 0x06u);
    CHECK(h.flag_C());   // C=1 means no borrow
    CHECK(!h.flag_N());
    CHECK(!h.flag_Z());
}

// ─── INX / DEX ───────────────────────────────────────────────────────────────

TEST_CASE(inx_dex)
{
    // $0400: A2 00  LDX #$00  (2)
    // $0402: E8     INX       (2)  → X=1
    // $0403: E8     INX       (2)  → X=2
    // $0404: CA     DEX       (2)  → X=1
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA2, 0x00, 0xE8, 0xE8, 0xCA});
    jmp_self(h, 0x0405);
    h.do_reset();
    h.run_cycles(2 + 2 + 2 + 2);

    CHECK_EQ(h.get_xr(), 0x01u);
}

// ─── INX wraps around 0xFF → 0x00 (zero flag) ────────────────────────────────

TEST_CASE(inx_wrap_sets_zero)
{
    // $0400: A2 FF  LDX #$FF  (2)
    // $0402: E8     INX       (2)  → X=0, Z=1
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA2, 0xFF, 0xE8});
    jmp_self(h, 0x0403);
    h.do_reset();
    h.run_cycles(4);

    CHECK_EQ(h.get_xr(), 0x00u);
    CHECK(h.flag_Z());
    CHECK(!h.flag_N());
}

// ─── AND / ORA / EOR ─────────────────────────────────────────────────────────

TEST_CASE(and_ora_eor)
{
    // AND: $AA & $0F = $0A
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0xAA,   // LDA #$AA
                     0x29, 0x0F}); // AND #$0F
    h.do_reset();
    h.run_cycles(4);
    CHECK_EQ(h.get_ac(), 0x0Au);

    // ORA: $0A | $50 = $5A
    h.write(0x0400, {0xA9, 0x0A,   // LDA #$0A
                     0x09, 0x50}); // ORA #$50
    h.do_reset();
    h.run_cycles(4);
    CHECK_EQ(h.get_ac(), 0x5Au);

    // EOR: $FF ^ $0F = $F0
    h.write(0x0400, {0xA9, 0xFF,   // LDA #$FF
                     0x49, 0x0F}); // EOR #$0F
    h.do_reset();
    h.run_cycles(4);
    CHECK_EQ(h.get_ac(), 0xF0u);
}

// ─── Branch – BEQ not taken ───────────────────────────────────────────────────

TEST_CASE(beq_not_taken)
{
    // $0400: A9 01  LDA #$01  (Z=0, N=0)  (2)
    // $0402: F0 02  BEQ +2    (not taken)  (2)
    // $0404: A9 55  LDA #$55               (2)
    // $0406: 4C 06 04 JMP $0406
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x01,         // LDA #$01
                     0xF0, 0x02,         // BEQ +2
                     0xA9, 0x55});       // LDA #$55
    jmp_self(h, 0x0406);
    h.do_reset();
    h.run_cycles(2 + 2 + 2);   // LDA + BEQ(not taken) + LDA

    CHECK_EQ(h.get_ac(), 0x55u);
}

// ─── Branch – BEQ taken ───────────────────────────────────────────────────────

TEST_CASE(beq_taken)
{
    // $0400: A9 00  LDA #$00   (Z=1)  (2)
    // $0402: F0 02  BEQ +2     (to $0406, skip LDA #$FF)  (3)
    // $0404: A9 FF  LDA #$FF   (skipped)
    // $0406: A9 AA  LDA #$AA   (2)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x00,         // LDA #$00  Z=1
                     0xF0, 0x02,         // BEQ +2
                     0xA9, 0xFF,         // should be skipped
                     0xA9, 0xAA});       // LDA #$AA
    jmp_self(h, 0x0408);
    h.do_reset();
    h.run_cycles(2 + 3 + 2);  // LDA + BEQ(taken) + LDA

    CHECK_EQ(h.get_ac(), 0xAAu);
}

// ─── Branch – BNE taken ───────────────────────────────────────────────────────

TEST_CASE(bne_taken)
{
    // Simple countdown loop: LDX #3, DEX, BNE -3
    // $0400: A2 03  LDX #$03  (2)
    // $0402: CA     DEX       (2)     ┐ loop
    // $0403: D0 FD  BNE $0402 (3/2)  ┘
    // After 3 DEX + 3 BNE(taken) + 1 BNE(not taken): X=0, Z=1
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA2, 0x03,   // LDX #3
                     0xCA,          // DEX   ← loop start
                     0xD0, 0xFD}); // BNE -3
    jmp_self(h, 0x0405);
    h.do_reset();
    // LDX(2) + 3×(DEX(2)+BNE_taken(3)) + DEX(2) + BNE_notaken(2) = 2+15+4=21
    h.run_cycles(21);

    CHECK_EQ(h.get_xr(), 0x00u);
    CHECK(h.flag_Z());
}

// ─── ASL accumulator ─────────────────────────────────────────────────────────

TEST_CASE(asl_accumulator)
{
    // $0400: A9 41  LDA #$41  (2)
    // $0402: 0A     ASL A     (2)  → $82, N=1, C=0
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x41, 0x0A});
    h.do_reset();
    h.run_cycles(4);

    CHECK_EQ(h.get_ac(), 0x82u);
    CHECK(h.flag_N());
    CHECK(!h.flag_C());
}

TEST_CASE(asl_shifts_bit7_into_carry)
{
    // $0400: A9 80  LDA #$80  (2)
    // $0402: 0A     ASL A     (2)  → $00, C=1, Z=1, N=0
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x80, 0x0A});
    h.do_reset();
    h.run_cycles(4);

    CHECK_EQ(h.get_ac(), 0x00u);
    CHECK(h.flag_C());
    CHECK(h.flag_Z());
    CHECK(!h.flag_N());
}

// ─── LSR accumulator ─────────────────────────────────────────────────────────

TEST_CASE(lsr_accumulator)
{
    // $0400: A9 81  LDA #$81  (2)
    // $0402: 4A     LSR A     (2)  → $40, C=1, N=0
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x81, 0x4A});
    h.do_reset();
    h.run_cycles(4);

    CHECK_EQ(h.get_ac(), 0x40u);
    CHECK(h.flag_C());
    CHECK(!h.flag_N());
}

// ─── ROL / ROR ───────────────────────────────────────────────────────────────

TEST_CASE(rol_carry_in)
{
    // SEC then ROL: C=1 rotated into bit 0
    // $0400: A9 40  LDA #$40  (2)
    // $0402: 38     SEC       (2)
    // $0403: 2A     ROL A     (2)  → $81 (bit0 = old C=1, bit7 = old bit6=1→ no, 0x40<<1 | 1 = 0x81)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x40, 0x38, 0x2A});
    h.do_reset();
    h.run_cycles(6);

    CHECK_EQ(h.get_ac(), 0x81u);
    CHECK(!h.flag_C());  // old bit7 of $40 was 0 → C=0
}

// ─── Stack: PHA / PLA ────────────────────────────────────────────────────────

TEST_CASE(pha_pla)
{
    // $0400: A9 42  LDA #$42  (2)
    // $0402: 48     PHA       (3)
    // $0403: A9 00  LDA #$00  (2)  – clear A
    // $0405: 68     PLA       (4)  – restore A
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x42,   // LDA #$42
                     0x48,          // PHA
                     0xA9, 0x00,   // LDA #$00
                     0x68});        // PLA
    jmp_self(h, 0x0406);
    h.do_reset();
    h.run_cycles(2 + 3 + 2 + 4);

    CHECK_EQ(h.get_ac(), 0x42u);
}

// ─── Stack: PHP / PLP ────────────────────────────────────────────────────────

TEST_CASE(php_plp_carry_flag)
{
    // Set carry via SEC, push SR, clear carry via CLC, restore SR via PLP
    // $0400: 38     SEC   (2)
    // $0401: 08     PHP   (3)
    // $0402: 18     CLC   (2)
    // $0403: 28     PLP   (4)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0x38, 0x08, 0x18, 0x28});
    jmp_self(h, 0x0404);
    h.do_reset();
    h.run_cycles(2 + 3 + 2 + 4);

    CHECK(h.flag_C());  // carry restored from stack
}

// ─── JSR / RTS ───────────────────────────────────────────────────────────────

TEST_CASE(jsr_rts)
{
    // $0400: 20 08 04  JSR $0408  (6) – call subroutine
    // $0403: A9 BB     LDA #$BB   (2) – executed after RTS
    // $0405: 4C 03 04  JMP $0403      – prevent run-off after LDA #$BB
    // $0408: A9 55     LDA #$55   (2) – subroutine body
    // $040A: 60        RTS        (6)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0x20, 0x08, 0x04,  // JSR $0408
                     0xA9, 0xBB,         // LDA #$BB
                     0x4C, 0x03, 0x04,  // JMP $0403
                     0xA9, 0x55,         // LDA #$55 (subroutine)
                     0x60});             // RTS
    h.do_reset();
    h.run_cycles(6 + 2 + 6);  // JSR + LDA (sub) + RTS

    // After RTS, PC is back at $0403 and LDA #$BB hasn't run yet
    h.run_cycles(2);  // LDA #$BB
    CHECK_EQ(h.get_ac(), 0xBBu);
}

// ─── TAX / TXA / TAY / TYA ───────────────────────────────────────────────────

TEST_CASE(register_transfers)
{
    // $0400: A9 37  LDA #$37  (2)
    // $0402: AA     TAX       (2)  A→X
    // $0403: A8     TAY       (2)  A→Y
    // $0404: A9 00  LDA #$00  (2)  clear A
    // $0406: 8A     TXA       (2)  X→A
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x37, 0xAA, 0xA8, 0xA9, 0x00, 0x8A});
    jmp_self(h, 0x0407);
    h.do_reset();
    h.run_cycles(2 + 2 + 2 + 2 + 2);

    CHECK_EQ(h.get_ac(), 0x37u);
    CHECK_EQ(h.get_xr(), 0x37u);
    CHECK_EQ(h.get_yr(), 0x37u);
}

// ─── CMP – equality ───────────────────────────────────────────────────────────

TEST_CASE(cmp_equal)
{
    // $0400: A9 55  LDA #$55  (2)
    // $0402: C9 55  CMP #$55  (2)  → Z=1, C=1 (no borrow), N=0
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0xA9, 0x55, 0xC9, 0x55});
    h.do_reset();
    h.run_cycles(4);

    CHECK(h.flag_Z());
    CHECK(h.flag_C());
    CHECK(!h.flag_N());
}

// ─── IRQ handling ─────────────────────────────────────────────────────────────

TEST_CASE(irq_jumps_to_vector)
{
    // IRQ handler at $0300: LDA #$CC, RTI
    // Main code at $0400:
    //   CLI  (2)  – clear I flag to enable IRQs
    //   NOP  (2)  – padding so IRQ fires after CLI
    //   NOP  (2)
    //   JMP $0404  – infinite loop
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.set_irq_vector(0x0300);

    // IRQ handler: load a marker value into A then return
    h.write(0x0300, {0xA9, 0xCC,  // LDA #$CC
                     0x40});       // RTI

    // Main code
    h.write(0x0400, {0x58,         // CLI  – enable IRQs
                     0xEA,          // NOP
                     0xEA,          // NOP
                     0x4C, 0x03, 0x04}); // JMP $0403

    h.do_reset();

    // Run CLI (2 cycles)
    h.run_cycles(2);

    // Fire IRQ; the CPU samples it on the next opcode boundary
    h.cpu.TriggerInterrupt(CIA_IRQ);

    // Run enough cycles: IRQ recognised at next opcode (NOP fetch),
    // then 7-cycle IRQ sequence, then 2-cycle LDA #$CC in handler
    h.run_cycles(2 + 7 + 2);

    CHECK_EQ(h.get_ac(), 0xCCu);
}

// ─── NMI handling ─────────────────────────────────────────────────────────────

TEST_CASE(nmi_jumps_to_vector)
{
    // NMI handler at $0200: LDA #$EE, RTI
    // Main code at $0400: LDA #$00, NOP, NOP, JMP $0403
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.set_nmi_vector(0x0200);

    h.write(0x0200, {0xA9, 0xEE, 0x40}); // LDA #$EE, RTI

    h.write(0x0400, {0xA9, 0x00,         // LDA #$00
                     0xEA, 0xEA,          // NOP NOP
                     0x4C, 0x02, 0x04}); // JMP $0402

    h.do_reset();
    h.run_cycles(2);  // LDA #$00

    // NMI is edge-triggered; fire it now
    h.cpu.TriggerInterrupt(CIA_NMI);

    // NOP fetch (2) + NMI sequence (7) + LDA #$EE in handler (2)
    h.run_cycles(2 + 7 + 2);

    CHECK_EQ(h.get_ac(), 0xEEu);
}

// ─── SEI disables IRQ ─────────────────────────────────────────────────────────

TEST_CASE(sei_masks_irq)
{
    // After SEI the IRQ flag (I=1) prevents the IRQ from being serviced.
    // The main loop LDA #$FF sets A=$FF; if the handler ran, A would be $CC.
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.set_irq_vector(0x0300);

    h.write(0x0300, {0xA9, 0xCC, 0x40}); // LDA #$CC, RTI

    h.write(0x0400, {0x78,              // SEI  – disable IRQs
                     0xA9, 0xFF,         // LDA #$FF
                     0x4C, 0x03, 0x04}); // JMP $0403

    h.do_reset();
    h.run_cycles(2);  // SEI

    h.cpu.TriggerInterrupt(CIA_IRQ);
    h.run_cycles(2 + 40);  // LDA #$FF + lots of idle loop cycles

    // A must still be $FF – IRQ was masked
    CHECK_EQ(h.get_ac(), 0xFFu);
}

// ─── Absolute addressing ──────────────────────────────────────────────────────

TEST_CASE(lda_absolute)
{
    // Place $DA at address $1234 and load it
    // $0400: AD 34 12  LDA $1234  (4 cycles)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.ram[0x1234] = 0xDA;
    h.write(0x0400, {0xAD, 0x34, 0x12}); // LDA $1234
    jmp_self(h, 0x0403);
    h.do_reset();
    h.run_cycles(4);

    CHECK_EQ(h.get_ac(), 0xDAu);
}

// ─── Indexed addressing ───────────────────────────────────────────────────────

TEST_CASE(lda_absolute_x)
{
    // $0400: A2 05     LDX #$05      (2)
    // $0402: BD 00 20  LDA $2000,X   (4, no page cross)
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.ram[0x2005] = 0xBE;
    h.write(0x0400, {0xA2, 0x05,
                     0xBD, 0x00, 0x20}); // LDA $2000,X
    jmp_self(h, 0x0405);
    h.do_reset();
    h.run_cycles(2 + 4);

    CHECK_EQ(h.get_ac(), 0xBEu);
}

// ─── JMP indirect ─────────────────────────────────────────────────────────────

TEST_CASE(jmp_indirect)
{
    // $0400: 6C 00 03  JMP ($0300)   vector = $0405
    // $0402: A9 FF     LDA #$FF  (should be skipped)
    // $0405: A9 5A     LDA #$5A
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.ram[0x0300] = 0x05;
    h.ram[0x0301] = 0x04;  // vector → $0405
    h.write(0x0400, {0x6C, 0x00, 0x03,  // JMP ($0300)
                     0xA9, 0xFF,          // skipped
                     0xA9, 0x5A});        // LDA #$5A
    jmp_self(h, 0x0407);
    h.do_reset();
    h.run_cycles(5 + 2);  // JMP ind (5) + LDA (2)

    CHECK_EQ(h.get_ac(), 0x5Au);
}

// ─── CLC / SEC / CLV ──────────────────────────────────────────────────────────

TEST_CASE(flag_instructions)
{
    // SEC → C=1; CLC → C=0; confirm
    CpuTestHarness h;
    h.set_reset_vector(0x0400);
    h.write(0x0400, {0x38, 0x18});  // SEC, CLC
    h.do_reset();
    h.run_cycles(2);  // SEC
    CHECK(h.flag_C());
    h.run_cycles(2);  // CLC
    CHECK(!h.flag_C());
}

TEST_MAIN()
