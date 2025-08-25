#pragma once

#include <fmt/print.h>

#ifndef ENABLE_ASSERTIONS
#define ENABLE_ASSERTIONS
#endif

#ifdef ENABLE_ASSERTIONS
#define assert(condition, ...)                                                                     \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            fmt::println("Assertion failed: ",                                                     \
                         #condition,                                                               \
                         " at ",                                                                   \
                         __FILE__,                                                                 \
                         ":",                                                                      \
                         __LINE__,                                                                 \
                         " with message:");                                                        \
            __VA_OPT__(fmt::println(__VA_ARGS__));                                                 \
            for (;;);                                                                              \
        }                                                                                          \
    } while (0)

#define assert_err(err)                                                                            \
    do {                                                                                           \
        static_assert(std::is_same_v<decltype(err), error>, "err must be an error");               \
        if (err.is_err()) {                                                                        \
            fmt::println("Error occured at ", __FILE__, ":", __LINE__, ": ", err.str());           \
            fmt::println(err.str());                                                               \
        }                                                                                          \
    } while (0)
#else
#define assert(condition, ...) ((void)0)
#define assert_err(condition, err) ((void)0)
#endif

#define todo(...)                                                                                  \
    do {                                                                                           \
        fmt::println("TODO: at ", __FILE__, ":", __LINE__, " with message:");                      \
        __VA_OPT__(fmt::println(__VA_ARGS__));                                                     \
        for (;;) {                                                                                 \
        }                                                                                          \
    } while (0)
