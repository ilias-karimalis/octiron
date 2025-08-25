#pragma once

// #include <freestanding.hpp>
#include <concepts>
#include <cstddef>
#include <cstdint>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using u128 = unsigned __int128;

using f32 = float;
using f64 = double;

using paddr_t = u64;
using vaddr_t = u64;

using ssize_t = i64;

static constexpr size_t
align_up(size_t value, size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

template<typename T>
static constexpr T*
align_up(T* value, size_t alignment)
{
    return (T*)align_up((size_t)value, alignment);
}

static constexpr size_t
align_down(size_t value, size_t alignment)
{
    return value & ~(alignment - 1);
}

template<typename T>
static constexpr T*
align_down(T* value, size_t alignment)
{
    return (T*)align_down((size_t)value, alignment);
}

static constexpr bool
is_aligned(size_t value, size_t alignment)
{
    return 0 == value % alignment;
}

template<typename T>
static constexpr bool
is_aligned(T* value, size_t alignment)
{
    return is_aligned((size_t)value, alignment);
}

#define ROUND_DOWN(value, alignment) ((value) & ~((alignment) - 1))
#define ROUND_UP(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))

// Define a concept for unsigned arithmetic	types.
template<typename T>
concept Unsigned = std::is_unsigned_v<T>;

// Define a concept for signed arithmetic types.
template<typename T>
concept Signed = std::is_signed_v<T>;

namespace num {

/// Minumum function
template<Unsigned T>
constexpr T
min(T lhs, T rhs)
{
    return (lhs < rhs) ? lhs : rhs;
}

inline u16
flip_endianness(u16 num)
{
    return (num >> 8) | (num << 8);
}

inline u32
flip_endianness(u32 num)
{
    return ((num >> 24) & 0x000000FF) | ((num >> 8) & 0x0000FF00) | ((num << 8) & 0x00FF0000) |
           ((num << 24) & 0xFF000000);
}

inline u64
flip_endianness(u64 num)
{
    return ((num >> 56) & 0x00000000000000FFULL) | ((num >> 40) & 0x000000000000FF00ULL) |
           ((num >> 24) & 0x0000000000FF0000ULL) | ((num >> 8) & 0x00000000FF000000ULL) |
           ((num << 8) & 0x000000FF00000000ULL) | ((num << 24) & 0x0000FF0000000000ULL) |
           ((num << 40) & 0x00FF000000000000ULL) | ((num << 56) & 0xFF00000000000000ULL);
}

inline u128
flip_endianness(u128 num)
{
    union
    {
        u128 val;
        u8 bytes[16];
    } src, dst;

    src.val = num;
    for (int i = 0; i < 16; i++) {
        dst.bytes[i] = src.bytes[15 - i];
    }
    return dst.val;
}

template<Unsigned U>
U
read_big_endian(const void* ptr)
{
    return flip_endianness(*(U*)(ptr));
}

} // namespace num