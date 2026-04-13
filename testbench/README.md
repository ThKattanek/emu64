# Emu64 Testbench

Standalone, GUI-free test infrastructure for the core MOS chip emulations in
Emu64.  Tests can be built and run without Qt, SDL, or any other GUI toolkit –
just a C++17 compiler and CMake.

---

## Directory layout

```
testbench/
├── CMakeLists.txt              – standalone build (no Qt, no SDL)
├── README.md                   – this file
├── framework/
│   └── test_framework.h        – minimal single-header test framework
├── mock/
│   ├── cpu_harness.h           – MOS6510 test harness (flat-RAM mock bus)
│   ├── cia_harness.h           – MOS6526 test harness
│   └── vic_harness.h           – VICII (MOS6569) test harness
├── test_cpu_6510.cpp           – CPU unit tests
├── test_cia_6526.cpp           – CIA unit tests
├── test_vic_6569.cpp           – VIC unit tests
└── functional_test_runner.cpp  – headless Klaus Dormann 6510 functional-test runner
```

---

## Quick start

```bash
cmake -S testbench -B build-tests -DCMAKE_BUILD_TYPE=Debug
cmake --build build-tests --parallel 4
ctest --test-dir build-tests --output-on-failure
```

---

## What is tested

### CPU (MOS 6510) – `test_cpu_6510`

| Test | What it verifies |
|------|-----------------|
| `lda_immediate_nonzero` | LDA # sets AC, clears Z, sets N when bit 7 set |
| `lda_immediate_zero` | LDA #0 sets Z flag |
| `sta_lda_roundtrip` | STA / LDA memory round-trip |
| `adc_no_carry` | ADC without carry |
| `adc_carry_out` | ADC carry output |
| `adc_signed_overflow` | ADC signed-overflow flag (V) |
| `sbc_basic` | SBC with borrow |
| `inx_dex` | INX / DEX |
| `inx_wrap_sets_zero` | INX wraps $FF→$00 and sets Z |
| `and_ora_eor` | AND / ORA / EOR |
| `beq_not_taken` | BEQ not taken |
| `beq_taken` | BEQ taken (same page) |
| `bne_taken` | BNE taken |
| `asl_accumulator` | ASL A |
| `asl_shifts_bit7_into_carry` | ASL: bit 7 → C |
| `lsr_accumulator` | LSR A |
| `rol_carry_in` | ROL with carry in |
| `pha_pla` | PHA / PLA stack round-trip |
| `php_plp_carry_flag` | PHP / PLP preserves C flag |
| `jsr_rts` | JSR / RTS call-return |
| `register_transfers` | TAX, TAY, TXA, TYA |
| `cmp_equal` | CMP sets Z and C for equal values |
| `irq_jumps_to_vector` | CIA IRQ dispatches to $FFFE/$FFFF |
| `nmi_jumps_to_vector` | NMI dispatches to $FFFA/$FFFB |
| `sei_masks_irq` | SEI prevents IRQ from firing |
| `lda_absolute` | LDA $FFFF (absolute addressing) |
| `lda_absolute_x` | LDA $FF00,X (absolute,X with page cross) |
| `jmp_indirect` | JMP ($FF00) indirect |
| `flag_instructions` | SEC/CLC/SEI/CLI/SED/CLD/CLV |

### CIA (MOS 6526) – `test_cia_6526`

| Test | What it verifies |
|------|-----------------|
| `timer_a_counts_down` | Timer A decrements once per phi2 cycle |
| `timer_a_oneshot_underflow` | One-shot: IRQ fires on underflow |
| `timer_a_oneshot_stops` | One-shot: only one IRQ fires |
| `timer_a_continuous_fires_multiple` | Continuous: IRQ fires multiple times |
| `icr_cleared_after_read` | Reading ICR clears it |
| `timer_b_counts_down` | Timer B decrements once per phi2 cycle |
| `timer_b_oneshot_underflow` | Timer B one-shot IRQ |
| `timer_a_latch_reload` | Timer reloads from latch after underflow |
| `timer_a_no_irq_when_masked` | No IRQ when ICR mask bit is clear |
| `tod_register_write_read` | TOD write/read round-trip (BCD) |
| `timer_a_16bit_latch` | 16-bit latch value round-trip |

### VIC-II (MOS 6569) – `test_vic_6569`

| Test | What it verifies |
|------|-----------------|
| `register_sprite0_x/y` | Sprite coordinate registers |
| `register_border/background_colour` | Colour registers (lower 4 bits) |
| `register_sprite_enable` | $D015 sprite-enable register |
| `register_ctrl1_scroll_and_rsel` | $D011 vertical-scroll / RSEL / DEN |
| `register_memory_pointers` | $D018 video-matrix / char-base |
| `register_irq_raster_latch` | $D012 read returns raster counter; write stored via GetRegister |
| `rasterline_increments_after_63_cycles` | PAL: exactly 63 cycles per raster line |
| `full_frame_312_lines` | PAL: 312 × 63 = 19 656 cycles/frame |
| `bad_line_range_starts_at_0x30` | BA goes low when bad-line DMA fires at $33 |
| `raster_irq_fires_at_configured_line` | Raster IRQ callback at $D012 compare |
| `sprite_x_msb_register` | $D010 sprite X MSB register |
| `den_bit_readable` | DEN bit (display enable) in $D011 |

---

## Klaus Dormann 6510 Functional Test (optional)

The `functional_test_runner` loads a flat 64 KB binary into emulated RAM, resets
the CPU, and runs until the PC loops (branch-to-self).

**Expected outcome:**  
`PC = $3469` → all sub-tests passed (exit 0)  
`PC = anything else` → a sub-test failed (exit 1)

### Obtaining the test binary

```bash
git clone https://github.com/Klaus2m5/6502_65C02_functional_tests
cd 6502_65C02_functional_tests
# Assemble with xa (or any 6502 assembler):
xa 6510_functional_tests.asm -o 6510_functional_test.bin
```

Or download a pre-built binary from the repository's GitHub Releases page.

### Running it

```bash
./build-tests/functional_test_runner path/to/6510_functional_test.bin
```

---

## Adding new tests

1. Create a new `test_<chip>.cpp` next to the existing ones.
2. `#include "framework/test_framework.h"` and the appropriate harness header.
3. Add a `TEST_CASE(name)` block and assertions using `CHECK`, `CHECK_EQ`, etc.
4. End the file with `TEST_MAIN()`.
5. Add the new executable to `CMakeLists.txt` and register it with `add_test`.

---

## Roadmap

The current tests establish a regression baseline.  Future phases (from the
compatibility roadmap) that build on this infrastructure:

- **Phase 2 CPU**: Run Klaus Dormann functional test in CI (once binary is bundled).
- **Phase 3 CIA**: Add cycle-accurate IRQ-delay tests; run VICE CIA test ROMs headless.
- **Phase 4 VIC-II**: Add border-trick and sprite-collision tests; run VICE VIC ROMs.
- **Phase 5 SID**: Envelope-generator timing tests; compare with SIDdump reference output.
