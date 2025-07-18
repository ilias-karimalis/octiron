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

static constexpr size_t
align_down(size_t value, size_t alignment)
{
    return value & ~(alignment - 1);
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

}