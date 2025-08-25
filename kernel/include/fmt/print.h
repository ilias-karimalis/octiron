#pragma once

#include <fmt/printer.h>
#include <types/error.h>

namespace fmt {
/// Initializes the formatted print library to print out characters using the given function.
error
initialize(putCharFunc fn);

/// Prints a sequence of printable types.
template<Printable... Args>
void
print(Args... args)
{
    print_with_func(detail::put_char, args...);
    // (printer<std::decay_t<Args>>::print(detail::put_char, std::forward<Args>(args)), ...);
}

/// Print a sequence of printable types, followed by a newline.
template<Printable... Args>
void
println(Args... args)
{
    println_with_func(detail::put_char, args...);
    // (printer<std::decay_t<Args>>::print(detail::put_char, std::forward<Args>(args)), ...);
    // detail::put_char('\n');
}
}