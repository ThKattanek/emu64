cmake_minimum_required(VERSION 3.14)
project(emu64_testbench CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Chip headers and implementations live in ../src
set(SRC_DIR "${CMAKE_SOURCE_DIR}/../src")
include_directories("${SRC_DIR}")
include_directories("${CMAKE_SOURCE_DIR}")

# ── Source sets (standalone – no Qt, no SDL) ─────────────────────────────────

set(CPU_SOURCES
    "${SRC_DIR}/mos6510_class.cpp")

set(CIA_SOURCES
    "${SRC_DIR}/mos6526_class.cpp"
    "${SRC_DIR}/mos6526_port.cpp")

set(VIC_SOURCES
    "${SRC_DIR}/mos6569_class.cpp")

# ── Compiler options ──────────────────────────────────────────────────────────

# Treat most warnings as errors to catch issues early.
# Suppress a few known-noisy warnings in the legacy chip source code.
set(TEST_WARN_FLAGS -Wall -Wextra -Wshadow)

# Some compilers warn about unused parameters in the chip sources; suppress
# those to keep build output clean.
set(CHIP_SUPPRESS_FLAGS
    -Wno-unused-parameter
    -Wno-sign-compare)

# ── test_cpu_6510 ─────────────────────────────────────────────────────────────

add_executable(test_cpu_6510
    test_cpu_6510.cpp
    ${CPU_SOURCES})

target_compile_options(test_cpu_6510 PRIVATE
    ${TEST_WARN_FLAGS} ${CHIP_SUPPRESS_FLAGS})

# ── test_cia_6526 ─────────────────────────────────────────────────────────────

add_executable(test_cia_6526
    test_cia_6526.cpp
    ${CIA_SOURCES})

target_compile_options(test_cia_6526 PRIVATE
    ${TEST_WARN_FLAGS} ${CHIP_SUPPRESS_FLAGS})

# ── test_vic_6569 ─────────────────────────────────────────────────────────────

add_executable(test_vic_6569
    test_vic_6569.cpp
    ${VIC_SOURCES})

target_compile_options(test_vic_6569 PRIVATE
    ${TEST_WARN_FLAGS} ${CHIP_SUPPRESS_FLAGS})

# ── functional_test_runner ────────────────────────────────────────────────────
# Requires the Klaus Dormann 6510 functional test binary (not included in the
# repository).  See testbench/README.md for instructions on how to obtain it.

add_executable(functional_test_runner
    functional_test_runner.cpp
    ${CPU_SOURCES})

target_compile_options(functional_test_runner PRIVATE
    ${TEST_WARN_FLAGS} ${CHIP_SUPPRESS_FLAGS})

# ── CTest integration ─────────────────────────────────────────────────────────

enable_testing()

add_test(NAME cpu_unit_tests COMMAND test_cpu_6510)
add_test(NAME cia_unit_tests COMMAND test_cia_6526)
add_test(NAME vic_unit_tests COMMAND test_vic_6569)

# The functional test is an optional, external-binary-dependent test.
# Enable it by running:
#   ctest -R functional_6510 -D CTEST_TESTBIN=<path-to-binary>
# or simply:
#   ./functional_test_runner <path-to-binary>
