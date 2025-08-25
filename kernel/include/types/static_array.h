#pragma once

#include <types/number.h>

template<typename T, size_t N>
class static_array
{
public:
    /// Default constructs the array elements.
    constexpr static_array() noexcept = default;

    /// Constructor that takes a single value and copies it into each index of
    /// the array.
    constexpr static_array(const T& val) noexcept;

    /// Constructor that takes an array literal
    consteval static_array(const T (&list)[N]) noexcept;

    /// Accesses specified element
    constexpr T& operator[](size_t index);
    constexpr const T& operator[](size_t index) const;

    /// Checks whether the container is empty
    constexpr bool empty() const noexcept;

    /// Returns the number of elements
    constexpr size_t size() const noexcept;

private:
    /// The underlying C-like array instance.
    T m_data[N];
};

template<typename T, size_t N>
constexpr static_array<T, N>::static_array(const T& val) noexcept
{
    for (size_t i = 0; i < N; i++) m_data[i] = val;
}

template<typename T, size_t N>
consteval static_array<T, N>::static_array(const T (&list)[N]) noexcept
  : m_data(list)
{
}

template<typename T, size_t N>
constexpr T&
static_array<T, N>::operator[](size_t index)
{
    return m_data[index];
}

template<typename T, size_t N>
constexpr const T&
static_array<T, N>::operator[](size_t index) const
{
    return m_data[index];
}

template<typename T, size_t N>
constexpr bool
static_array<T, N>::empty() const noexcept
{
    return N == 0;
}

template<typename T, size_t N>
constexpr size_t
static_array<T, N>::size() const noexcept
{
    return N;
}
