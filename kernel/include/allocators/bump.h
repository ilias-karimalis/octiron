/// Implementation of a non-freeing bump allocator. The allocated memory can only ever be returned
/// to the pmm by destroying the whole bump allocator.
#pragma once

#include <types/error.h>

class bump_alloc
{
public:
    /// Creates an empty bump allocator.
    constexpr bump_alloc() = default;

    /// Move constructor
    bump_alloc(bump_alloc&& other) noexcept;

    /// Frees the allocated memory.
    ~bump_alloc();

    /// Grows the allocator by one pages
    error grow();

    /// Grows the allocator by adding `n_pages` pages.
    error grow_by_n_pages(size_t n_pages);

    /// Allocates a block of byte-aligned memory of size `size`, returns NULL if the allocation
    /// fails.
    void* alloc(size_t size);

    /// Allocates a block of `alignment`-aligned memory of size `size`, returns NULL if the
    /// allocation fails.
    void* alloc_aligned(size_t size, size_t alignment);

    /// Move assignment
    bump_alloc& operator=(bump_alloc&& other) noexcept;

private:
    struct region
    {
        u8* end;
        u8* curr;
        struct region* next;
    };

    struct region* m_region_list;
    struct region* m_current_region;
};