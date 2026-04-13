// Klaus Dormann 6510 Functional Test Runner
//
// This headless runner loads the 6510 functional-test binary
// (6510_functional_test.bin) into a flat 64 KB RAM, performs a CPU reset, and
// runs the test to completion.
//
// Expected outcome:
//   PC loops at $3469  → all tests passed  (exit 0)
//   PC loops elsewhere → a sub-test failed (exit 1)
//
// How to obtain the test binary:
//   The test suite was written by Klaus Dormann and is available at
//   https://github.com/Klaus2m5/6502_65C02_functional_tests
//
//   Assemble for the 6510 (I/O port variant):
//     cd 6502_65C02_functional_tests
//     # Edit 6510_functional_tests.asm, set the desired options, then:
//     make   # or: xa 6510_functional_tests.asm -o 6510_functional_test.bin
//
//   A pre-built binary can also be downloaded as a release asset from the
//   GitHub repository above.
//
// Usage:
//   functional_test_runner <path-to-6510_functional_test.bin>
//
// The binary is exactly 65536 bytes and is loaded at $0000.
// Execution begins at $0400 (embedded in the reset vector at $FFFC/$FFFD).

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mock/cpu_harness.h"

// Trap address for a successful run (branch-to-self loop at the end of the test)
static constexpr uint16_t SUCCESS_PC = 0x3469;

// Maximum cycles to run before declaring a timeout (≈100 M cycles, ~100 s at
// real C64 speed; the test typically completes in far fewer cycles)
static constexpr uint64_t MAX_CYCLES = 100'000'000ULL;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,
                "Usage: %s <path-to-6510_functional_test.bin>\n\n"
                "Obtain the binary from:\n"
                "  https://github.com/Klaus2m5/6502_65C02_functional_tests\n",
                argv[0]);
        return 2;
    }

    const char* path = argv[1];
    FILE* f = fopen(path, "rb");
    if (!f)
    {
        fprintf(stderr, "Error: cannot open '%s'\n", path);
        return 2;
    }

    CpuTestHarness h;

    size_t n = fread(h.ram, 1, sizeof(h.ram), f);
    fclose(f);

    if (n != 65536)
    {
        fprintf(stderr,
                "Error: expected 65536 bytes, read %zu from '%s'\n",
                n, path);
        return 2;
    }

    printf("Loaded '%s' (%zu bytes)\n", path, n);
    printf("Reset vector: $%04X\n",
           static_cast<uint16_t>(h.ram[0xFFFC]) |
           (static_cast<uint16_t>(h.ram[0xFFFD]) << 8));

    // Perform reset; the reset vector in the binary ($FFFC/$FFFD) points to $0400
    h.do_reset();

    printf("Running functional test (max %llu cycles)...\n",
           (unsigned long long)MAX_CYCLES);

    uint16_t trapped_pc = h.run_until_trap(MAX_CYCLES, /*stable_cycles=*/8);

    printf("CPU trapped at PC = $%04X\n", trapped_pc);

    if (trapped_pc == SUCCESS_PC)
    {
        printf("PASS: all sub-tests completed successfully.\n");
        return 0;
    }
    else
    {
        printf("FAIL: test trapped at $%04X (expected $%04X).\n",
               trapped_pc, SUCCESS_PC);
        printf("Hint: look up $%04X in the test source to identify the "
               "failing sub-test.\n", trapped_pc);
        return 1;
    }
}
