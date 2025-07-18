#pragma once

#include <fmt/printer.hpp>
#include <types/error.hpp>

#include <utility>

namespace fmt {
/// Initializes the formatted print library to print out characters using the given function.
error
initialize(putCharFunc fn);

/// Defines a concept of types which can be printed, i.e. implement the `printer` class.
template<class T>
concept Printable = requires(T t) {
    { printer<T>::print(detail::put_char, t) } -> std::same_as<void>;
};

/// Prints a sequence of printable types.
template<Printable... Args>
void
print(Args... args)
{
    (printer<std::decay_t<Args>>::print(detail::put_char, std::forward<Args>(args)), ...);
}

/// Print a sequence of printable types, followed by a newline.
template<Printable... Args>
void
println(Args... args)
{
    (printer<std::decay_t<Args>>::print(detail::put_char, std::forward<Args>(args)), ...);
    detail::put_char('\n');
}
}