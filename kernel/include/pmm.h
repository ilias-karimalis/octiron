#pragma once

#include <allocators/slab.h>
#include <fmt/printer.h>
#include <types/error.h>

namespace pmm {

enum class Policy
{
    /// Allocates from the first region it finds which fits the allocation.
    FIRST_FIT,
    /// Allocates from the smallest possible region.
    BEST_FIT,
    /// Allocates from the largest possible region.
    WORST_FIT,
    /// Allocates from the first region it finds starting where the last
    /// allocation was.
    NEXT_FIT
};

void
initialize(Policy p);

/// Adds a new contiguous memory region to the physical memory manager.
error
add_region(paddr_t region_base, size_t region_size);

/// Removes a contiguous region from the physical memory manager.
error
remove_region(paddr_t region_base, size_t region_size);

/// Allocates a region from the physical memory manager with the requested size and alignment.
error
alloc_aligned(size_t size, size_t alignment, paddr_t* ret);

/// Allocate a region from the physical memory manager with the requested size and alignment, if
/// there's an error, it will return nullptr instead.
paddr_t
alloc_aligned_noerr(size_t size, size_t alignment);

/// Allocates a region of memory with the requested size and base page size alignment.
error
alloc(size_t size, paddr_t* ret);

/// Allocates a region of memory with the requested size and base page size alignment, if there's an
/// error, it will return nullptr instead.
paddr_t
alloc_noerr(size_t size);

/// Frees a previously allocated region of memory, returning the allocated memory back to the
/// physical memory manager.
error
free(paddr_t ret);

/// Returns the total amount of memory managed by the physical memory manager.
size_t
total_memory();

/// Returns the total amount of free memory managed by the physical memory manager.
size_t
free_memory();

}

template<>
struct fmt::printer<pmm::Policy>
{
    static void print(putCharFunc fn, const pmm::Policy& p)
    {
        switch (p) {
            case pmm::Policy::FIRST_FIT:
                printer<str_view>::print(fn, "pmm::policy::FIRST_FIT");
                break;
            case pmm::Policy::BEST_FIT:
                printer<str_view>::print(fn, "pmm::policy::BEST_FIT");
                break;
            case pmm::Policy::WORST_FIT:
                printer<str_view>::print(fn, "pmm::policy::WORST_FIT");
                break;
            case pmm::Policy::NEXT_FIT:
                printer<str_view>::print(fn, "pmm::policy::NEXT_FIT");
                break;
        }
    }
};
