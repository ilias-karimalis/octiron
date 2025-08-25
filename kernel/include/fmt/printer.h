#pragma once

#include <types/number.h>
#include <types/str_view.h>
#include <utility>

namespace fmt {
using putCharFunc = void (*)(char);

namespace detail {
extern putCharFunc put_char;
}

/// Defines a class for printing objects of type T, allowing for specialization by each type.
template<typename T>
struct printer
{
    static void print(putCharFunc fn, T obj);
};

// Printer implementation for unsigned types
template<Unsigned T>
struct printer<T>
{
    static void print(const putCharFunc fn, T value)
    {
        static_assert(m_base >= 2 || m_base <= 16,
                      "Unsupported Base used, must be between 2 and 16.");

        if (value == 0) {
            detail::put_char('0');
            return;
        }

        char static buffer[65] = { 0 };
        int index = 0;

        const char digits[] = "0123456789ABCDEF";

        T num = value;
        while (num > 0) {
            buffer[index++] = digits[num % m_base];
            num /= m_base;
        }

        for (int i = index - 1; i >= 0; i--) {
            fn(buffer[i]);
        }
    }

    /// We should have a way of printing different bases, for now this is just const.
    static constexpr int m_base = 10;
};

// Printer implementation for signed types
template<Signed T>
struct printer<T>
{
    static void print(putCharFunc fn, T value)
    {
        if (value < 0) {
            fn('-');
            printer<typename std::make_unsigned<T>::type>::print(
              fn, static_cast<std::make_unsigned<T>::type>(-value));
        } else {
            printer<typename std::make_unsigned<T>::type>::print(
              fn, static_cast<std::make_unsigned<T>::type>(value));
        }
    }
};

/// Printer implementation  for null-terminated strings.
template<>
struct printer<const char*>
{
    static void print(const putCharFunc fn, const char* str)
    {
        while (*str != '\0') {
            fn(*str++);
        }
    }
};

/// We allow for hexadecimal printing of unsigned arithmetic types through a specialization of the
/// printer class.
template<Unsigned T>
struct hex
{
    explicit constexpr hex(T value)
      : m_value(value)
    {
    }

    friend printer<hex>;
    T m_value;
};

/// Specialization of the printer class for hexadecimal printing of unsigned types.
template<Unsigned T>
struct printer<hex<T>>
{
    static void print(const putCharFunc fn, hex<T> value)
    {
        static_assert(sizeof(T) <= 8,
                      "Hexadecimal printing is only supported for types up to 64 bits.");
        printer<const char*>::print(fn, "0x");
        if (value.m_value == 0) {
            fn('0');
            return;
        }

        char static buffer[65] = { 0 };
        int index = 0;

        constexpr char digits[] = "0123456789ABCDEF";

        T num = value.m_value;
        while (num > 0) {
            buffer[index++] = digits[num % 16];
            num /= 16;
        }

        for (int i = index - 1; i >= 0; i--) {
            fn(buffer[i]);
        }
    }
};

/// Specialization of the printer class for pointer types.
template<typename T>
struct printer<T*>
{
    static void print(const putCharFunc fn, T* ptr)
    {
        printer<hex<size_t>>::print(fn, hex((size_t)ptr));
    }
};

/// printer specialization for str_view
template<>
class printer<str_view>
{
public:
    static void print(putCharFunc fn, const str_view& sv)
    {
        for (size_t i = 0; i < sv.length(); i++) {
            fn(sv[i]);
        }
    }
};

/// printer sp

/// Defines a concept of types which can be printed, i.e. implement the `printer` class.
template<class T>
concept Printable = requires(T t) {
    { printer<T>::print(detail::put_char, t) } -> std::same_as<void>;
};

/// Prints a sequence of printable types, with the given put_char function.
template<Printable... Args>
void
print_with_func(putCharFunc fn, Args... args)
{
    (printer<std::decay_t<Args>>::print(fn, std::forward<Args>(args)), ...);
}

/// Prints a sequence of printable types, followed by a newliine with the given put_char function.
template<Printable... Args>
void
println_with_func(putCharFunc fn, Args... args)
{
    print_with_func(fn, args...);
    fn('\n');
}
}