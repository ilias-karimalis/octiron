/// Kernel dynamic stack implementatioin, relies on the pmm and hhdm for memory allocation.
#pragma once

#include <fmt/assert.h>
#include <limine/platform_info.h>
#include <memory.h>
#include <new>
#include <pmm.h>
#include <riscv/sv39.h>
#include <types/error.h>
#include <types/number.h>

template<typename T>
struct stack
{
    /// Creates an empty stack.
    stack();

    /// Move constructor.
    stack(stack&& other) noexcept;

    /// Frees a stack
    ~stack();

    /// Grows a stack to be large enough to fit the given minimum capacity.
    error grow_to_min_cap(size_t minimum_capacity);

    /// Pushes a value onto the stack, returning the address of the pushed-back value.
    T* push_back(T value);

    /// Emplaces a value onto the stack, avoiding excess copys. Returns the address of the
    /// pushed-back value.
    template<typename... Args>
    T* emplace_back(Args&&... args);

    /// Accesses specified element.
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    /// Move assignment
    stack& operator=(stack&& other) noexcept;

    /// Grows a stack with a 3/2 growth rate.
    void grow();

    size_t m_size;
    size_t m_capacity;
    T* m_buffer;
};

template<typename T>
stack<T>::stack()
  : m_size(0)
  , m_capacity(0)
  , m_buffer(nullptr)
{
}

template<typename T>
stack<T>::stack(stack&& other) noexcept
  : m_size(other.m_size)
  , m_capacity(other.m_capacity)
  , m_buffer(other.m_buffer)
{
    other.m_size = 0;
    other.m_capacity = 0;
    other.m_buffer = nullptr;
}

template<typename T>
stack<T>::~stack()
{
    error err = pmm::free(limine::hhdm_virt_to_phys(m_buffer));
    assert_err(err);
}

template<typename T>
void
stack<T>::grow()
{
    if (m_size == m_capacity) {
        size_t current_size = align_up(m_capacity * sizeof(T), riscv::sv39::PAGE_SIZE);
        size_t new_size = align_up(1 + (current_size * 3) / 2, riscv::sv39::PAGE_SIZE);
        size_t new_capacity = new_size / sizeof(T);
        void* old_buffer = m_buffer;
        paddr_t new_pa = pmm::alloc_noerr(new_size);
        assert(new_pa != 0);
        void* new_buffer = limine::hhdm_phys_to_virt(new_pa);
        mem::copy(old_buffer, new_buffer, m_capacity * sizeof(T));
        error err = pmm::free(limine::hhdm_virt_to_phys(old_buffer));
        assert_err(err);

        m_capacity = new_capacity;
        m_buffer = (T*)new_buffer;
    }
}

template<typename T>
T*
stack<T>::push_back(T value)
{
    grow();
    m_buffer[m_size] = value;
    m_size++;
    return m_buffer + m_size - 1;
}

template<typename T>
template<typename... Args>
T*
stack<T>::emplace_back(Args&&... args)
{
    grow();
    T* obj = new (&m_buffer[m_size]) T{ std::forward<Args>(args)... };
    m_size++;
    return obj;
}

template<typename T>
T&
stack<T>::operator[](size_t index)
{
    return m_buffer[index];
}

template<typename T>
const T&
stack<T>::operator[](size_t index) const
{
    return m_buffer[index];
}

template<typename T>
stack<T>&
stack<T>::operator=(stack&& other) noexcept
{
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    m_buffer = other.m_buffer;
    other.m_size = 0;
    other.m_capacity = 0;
    other.m_buffer = nullptr;
    return *this;
}
