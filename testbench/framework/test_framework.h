// Minimal single-header test framework for Emu64 chip unit tests.
// Usage:
//   #include "framework/test_framework.h"
//   TEST_CASE(my_test) { CHECK_EQ(1 + 1, 2); }
//   TEST_MAIN()

#pragma once

#include <cstdio>
#include <functional>
#include <vector>
#include <string>

// ── Registry ────────────────────────────────────────────────────────────────

struct TestRegistry
{
    struct TestCase
    {
        const char*           name;
        std::function<void()> func;
    };

    std::vector<TestCase> tests;
    int                   pass_count    = 0;
    int                   fail_count    = 0;
    const char*           current_test  = "";

    static TestRegistry& instance()
    {
        static TestRegistry reg;
        return reg;
    }

    void add(const char* name, std::function<void()> func)
    {
        tests.push_back({name, func});
    }

    int run_all()
    {
        for (auto& tc : tests)
        {
            current_test = tc.name;
            printf("[ RUN  ] %s\n", tc.name);
            tc.func();
        }
        printf("\n=== Results: %d passed, %d failed ===\n", pass_count, fail_count);
        return fail_count > 0 ? 1 : 0;
    }
};

// ── Registration macro ───────────────────────────────────────────────────────

#define TEST_CASE(name)                                                         \
    static void _tc_body_##name();                                              \
    static int  _tc_reg_##name =                                                \
        (TestRegistry::instance().add(#name, _tc_body_##name), 0);             \
    static void _tc_body_##name()

// ── Assertion macros ─────────────────────────────────────────────────────────

#define CHECK(expr)                                                             \
    do {                                                                        \
        if (expr) {                                                             \
            ++TestRegistry::instance().pass_count;                              \
        } else {                                                                \
            ++TestRegistry::instance().fail_count;                              \
            fprintf(stderr, "  FAIL [%s:%d]: CHECK(%s)\n",                     \
                    TestRegistry::instance().current_test, __LINE__, #expr);   \
        }                                                                       \
    } while (0)

#define CHECK_EQ(a, b)                                                          \
    do {                                                                        \
        auto _a = (a);                                                          \
        auto _b = (b);                                                          \
        if (_a == _b) {                                                         \
            ++TestRegistry::instance().pass_count;                              \
        } else {                                                                \
            ++TestRegistry::instance().fail_count;                              \
            fprintf(stderr,                                                     \
                    "  FAIL [%s:%d]: CHECK_EQ(%s, %s)"                         \
                    "  got 0x%llX != 0x%llX\n",                                \
                    TestRegistry::instance().current_test, __LINE__,           \
                    #a, #b,                                                     \
                    (unsigned long long)_a, (unsigned long long)_b);           \
        }                                                                       \
    } while (0)

#define CHECK_NE(a, b)                                                          \
    do {                                                                        \
        auto _a = (a);                                                          \
        auto _b = (b);                                                          \
        if (_a != _b) {                                                         \
            ++TestRegistry::instance().pass_count;                              \
        } else {                                                                \
            ++TestRegistry::instance().fail_count;                              \
            fprintf(stderr,                                                     \
                    "  FAIL [%s:%d]: CHECK_NE(%s, %s)"                         \
                    "  both == 0x%llX\n",                                       \
                    TestRegistry::instance().current_test, __LINE__,           \
                    #a, #b, (unsigned long long)_a);                            \
        }                                                                       \
    } while (0)

// Like CHECK_EQ but aborts the current test on failure
#define REQUIRE_EQ(a, b)                                                        \
    do {                                                                        \
        auto _a = (a);                                                          \
        auto _b = (b);                                                          \
        if (_a == _b) {                                                         \
            ++TestRegistry::instance().pass_count;                              \
        } else {                                                                \
            ++TestRegistry::instance().fail_count;                              \
            fprintf(stderr,                                                     \
                    "  FAIL [%s:%d]: REQUIRE_EQ(%s, %s)"                       \
                    "  got 0x%llX != 0x%llX (aborting test)\n",                \
                    TestRegistry::instance().current_test, __LINE__,           \
                    #a, #b,                                                     \
                    (unsigned long long)_a, (unsigned long long)_b);           \
            return;                                                             \
        }                                                                       \
    } while (0)

#define REQUIRE(expr)                                                           \
    do {                                                                        \
        if (expr) {                                                             \
            ++TestRegistry::instance().pass_count;                              \
        } else {                                                                \
            ++TestRegistry::instance().fail_count;                              \
            fprintf(stderr,                                                     \
                    "  FAIL [%s:%d]: REQUIRE(%s) (aborting test)\n",           \
                    TestRegistry::instance().current_test, __LINE__, #expr);   \
            return;                                                             \
        }                                                                       \
    } while (0)

// ── main() helper ────────────────────────────────────────────────────────────

#define TEST_MAIN()                                                             \
    int main()                                                                  \
    {                                                                           \
        return TestRegistry::instance().run_all();                             \
    }
