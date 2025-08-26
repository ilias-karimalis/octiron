#pragma once

#include "memory.h"
#include "types/byte_view.h"
#include <cstring>
#include <types/number.h>

class str_view
{
public:
    consteval str_view()
      : m_data(nullptr)
      , m_size(0)
    {
    }

    /// Constructs a str_view at compile time from a string literal.
    template<size_t N>
    consteval str_view(const char (&literal)[N])
      : m_data(literal)
      , m_size(N - 1)
    {
        static_assert(N - 1 != s_sentinel,
                      "str_view can't represent a string this large (uses -1 for errors).");
    }

    /// Constructs a str_view from a const char pointer and a length.
    constexpr str_view(const char* str, size_t str_len)
      : m_data(str)
      , m_size(str_len)
    {
    }

    /// Constructs a str_view from a null-terminated string.
    static constexpr str_view from_null_term(const char* str)
    {
        //        size_t len = 0;
        //        for (const char* curr = str; *curr != '\0'; curr++, len++);
        return { str, mem::strlen(str) };
    }

    /// Constructs a str_view from a byte_view.
    static constexpr str_view from_byte_view(byte_view bv)
    {
        return { (const char*)bv.data(), bv.length() };
    }

    /// Accesses the i-th character.
    constexpr const char& operator[](size_t i) const { return m_data[i]; }

    /// Accesses the i-th character with assert based bound checking.
    constexpr const char& at(size_t i) const;

    // Returns a pointer to the underlying character array.
    constexpr const char* data() const noexcept { return m_data; }

    /// Returns the length of the str_view.
    constexpr size_t length() const { return m_size; }

    /// Returns a new str_view that is the current one advanced by `n` bytes.
    constexpr str_view advance(size_t n);

    /// Returns a new str_view that is the current one shortened by `n` bytes.
    constexpr str_view shorten(size_t n);

    /// Returns the str_view of a substring starting at `start` with length up to `len`.
    constexpr str_view substr(size_t start = 0, size_t len = SIZE_MAX) const;

    ///	Returns the index of first occurrence of `c` in the str_view, starting from `start`, or
    ///	`sentinel` if not found.
    constexpr size_t find(char c, size_t start = 0) const;

    static constexpr ssize_t compare(str_view s1, str_view s2);

    static constexpr size_t s_sentinel = static_cast<size_t>(-1);

private:
    /// Pointer to the underlying sequence.
    const char* m_data;
    /// The number of characters in the str_view.
    size_t m_size;
};

constexpr str_view
str_view::advance(size_t n)
{
    return { m_data + n, m_size };
}

constexpr str_view
str_view::shorten(size_t n)
{
    return { m_data, m_size - n };
}

constexpr str_view
str_view::substr(size_t start, size_t len) const
{
    if (start > m_size)
        return { m_data + m_size, 0 };
    return { m_data + start, num::min(len, m_size - start) };
}

constexpr size_t
str_view::find(char c, size_t start) const
{
    for (size_t i = start; i < m_size; i++) {
        if (m_data[i] == c) {
            return i;
        }
    }
    return s_sentinel;
}

constexpr ssize_t
str_view::compare(str_view s1, str_view s2)
{
    size_t i = 0;
    while (i < s1.m_size && i < s2.m_size) {
        if (s1.m_data[i] != s2.m_data[i])
            return static_cast<unsigned char>(s1.m_data[i]) -
                   static_cast<unsigned char>(s2.m_data[i]);
        ++i;
    }
    return static_cast<ssize_t>(s1.m_size) - static_cast<ssize_t>(s2.m_size);
}
