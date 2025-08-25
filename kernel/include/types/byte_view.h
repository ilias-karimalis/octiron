#pragma once

#include <cstring>
#include <types/number.h>

class byte_view
{
public:
    /// Constructs a byte_view at compile time from a string literal.
    template<size_t N>
    consteval byte_view(const u8 (&literal)[N])
      : m_data(literal)
      , m_size(N - 1)
    {
        static_assert(N - 1 != s_sentinel,
                      "byte_view can't represent a view this large (uses -1 for errors).");
    }

    /// Constructs a byte_view from a const u8 pointer and a length.
    constexpr byte_view(const u8* buf, size_t buf_len)
      : m_data(buf)
      , m_size(buf_len)
    {
    }

    /// Accesses the i-th character.
    constexpr const u8& operator[](size_t i) const { return m_data[i]; }

    // Returns a pointer to the underlying character array.
    constexpr const u8* data() const noexcept { return m_data; }

    /// Returns the length of the byte_view.
    constexpr size_t length() const { return m_size; }

    /// Shrinks the view by moving the start forward
    constexpr byte_view advance(size_t n);

    /// Shrinks the view by moving the end backwards
    constexpr byte_view shorten(size_t n);

    /// Returns the byte_view of a substring starting at `start` with length up to `len`.
    constexpr byte_view subview(size_t start = 0, size_t len = SIZE_MAX) const;

    ///	Returns the index of first occurrence of `val` in the byte_view, starting from `start`, or
    ///	`sentinel` if not found.
    constexpr size_t find(u8 val, size_t start = 0) const;

    static constexpr ssize_t compare(byte_view s1, byte_view s2);

    static constexpr size_t s_sentinel = static_cast<size_t>(-1);

private:
    /// Pointer to the underlying sequence.
    const u8* m_data;
    /// The number of characters in the byte_view.
    size_t m_size;
};

constexpr byte_view
byte_view::advance(size_t n)
{
    m_data += n;
    return *this;
}

constexpr byte_view
byte_view::shorten(size_t n)
{
    m_size -= n;
    return *this;
}

constexpr byte_view
byte_view::subview(size_t start, size_t len) const
{
    if (start > m_size)
        return { m_data + m_size, 0 };
    return { m_data + start, num::min(len, m_size - start) };
}

constexpr size_t
byte_view::find(u8 val, size_t start) const
{
    for (size_t i = start; i < m_size; i++) {
        if (m_data[i] == val) {
            return i;
        }
    }
    return s_sentinel;
}

constexpr ssize_t
byte_view::compare(byte_view s1, byte_view s2)
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
