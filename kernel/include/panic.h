#pragma once

#include <fmt/print.h>

/// Panics the kernel because of the provided reason.
template<fmt::Printable... Args>
void
panic(Args... args)
{
    fmt::println("[Kernel Panic]: ", args...);
    for (;;) {
        asm("wfi");
    }
}