/// Implementation of a non-freeing bump allocator
#pragma once

#include <types/number.hpp>

class bump_alloc
{
public:
    /// Initializes a bump allocator with a given region of memory
    bump_alloc(u8* buffer, size_t buffer_size);

    /// Allocates a block of memory of size `size`, returns NULL if the
    /// allocation fails.
    void* alloc(size_t size);

    /// Allocates a block of memory of size `size` with the given `aligment`,
    /// returns NULL if the allocation fails.
    void* alloc_aligned(size_t size, size_t alignment);

    /// Creates a copy of a block of memory, returning a pointer to the copy,
    /// returns NULL if there are any issues.
    void* copy(const void* src, size_t size);

    /// Creates a copy of a null-terminated string, returning a pointer to the
    /// copy, returns NULL if there are any issues.
    const char* str_copy(const char* src);

private:
    u8* m_base;
    size_t m_size;
    size_t m_index;
};
