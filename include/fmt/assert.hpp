#pragma once

#include <fmt/print.hpp>

#ifdef ENABLE_ASSERTIONS
#define assert(condition, ...)                                                                                         \
    do {                                                                                                               \
        if (!(condition)) {                                                                                            \
            fmt::println("Assertion failed: ", #condition, " at ", __FILE__, ":", __LINE__, " with message:");         \
            __VA_OPT__(fmt::println(__VA_ARGS__));                                                                     \
            for (;;)                                                                                                   \
                ;                                                                                                      \
        }                                                                                                              \
    } while (0)
#else
#define assert(condition, ...) ((void)0)
#endif

#define todo(...)                                                                                                      \
    do {                                                                                                               \
        fmt::println("TODO: at ", __FILE__, ":", __LINE__, " with message:");                                          \
        __VA_OPT__(fmt::println(__VA_ARGS__));                                                                         \
        for (;;) {                                                                                                     \
        }                                                                                                              \
    } while (0)
