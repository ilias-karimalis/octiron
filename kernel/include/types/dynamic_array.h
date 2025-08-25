// Kernel Dynamic Array collection, relies on the pmm and hhdm for memory allocations.
#pragma once

#include <fmt/assert.h>
#include <limine/platform_info.h>
#include <memory.h>
#include <pmm.h>
#include <riscv/sv39.h>
#include <types/error.h>
#include <types/number.h>
#include <types/result.h>

template<typename T>
class dynamic_array
{
public:
    /// Creates an empty dynamic array.
    dynamic_array();

    /// Creates a dynamic array by allocating a number of physical pages from the pmm
    static result<dynamic_array> create_with_min_size(size_t minimum_size);

    /// Frees a dynamic array, returning the pages to the pmm
    ~dynamic_array();

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    /// Asserts to test failure
    void push_back(T val);

    error grow();

    size_t m_capacity;
    size_t m_size;

    static_assert(sizeof(T) <= riscv::sv39::PAGE_SIZE);

private:
    T* m_buffer;
};

template<typename T>
dynamic_array<T>::dynamic_array()
  : m_capacity(0)
  , m_size(0)
  , m_buffer(nullptr)
{
}

template<typename T>
result<dynamic_array<T>>
dynamic_array<T>::create_with_min_size(size_t minimum_size)
{
    size_t size = align_up(minimum_size * sizeof(T), riscv::sv39::PAGE_SIZE);
    size_t count = size / sizeof(T);
    paddr_t pa;
    error err = pmm::alloc(size, &pa);
    if (err.is_err()) {
        return result<dynamic_array>::make_err(err.push(ErrorCode::DYN_ARR_ALLOC_FAILURE));
    }
    void* buffer = limine::hhdm_phys_to_virt(pa);
    return result<dynamic_array>::make_some({ .m_capacity = count, .m_buffer = buffer });
}

template<typename T>
dynamic_array<T>::~dynamic_array()
{
    todo("Implement this!");
}

template<typename T>
T&
dynamic_array<T>::operator[](size_t index)
{
    return m_buffer[index];
}

template<typename T>
const T&
dynamic_array<T>::operator[](size_t index) const
{
    return m_buffer[index];
}

template<typename T>
error
dynamic_array<T>::grow()
{
    size_t current_size = align_up(m_capacity * sizeof(T), riscv::sv39::PAGE_SIZE);
    size_t new_size = align_up((current_size * 3) / 2, riscv::sv39::PAGE_SIZE);
    size_t new_count = new_size / sizeof(T);
    void* old_buffer = m_buffer;
    paddr_t new_pa;
    error err = pmm::alloc(new_size, &new_pa);
    if (err.is_err()) {
        return err.push(ErrorCode::DYN_ARR_REALLOC_FAILURE);
    }
    void* new_buffer = limine::hhdm_phys_to_virt(new_pa);
    mem::copy(old_buffer, new_buffer, m_capacity * sizeof(T));
    err = pmm::free(limine::hhdm_virt_to_phys(old_buffer));
    if (err.is_err()) {
        return err.push(ErrorCode::DYN_ARR_REALLOC_FAILURE);
    }
    m_buffer = new_buffer;
    m_capacity = new_count;
}

template<typename T>
void
push_back(T value)
{
}