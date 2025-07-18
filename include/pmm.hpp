#pragma once

#include <colls/slab_alloc.hpp>
#include <fmt/printer.hpp>
#include <types/error.hpp>

namespace pmm {

enum class policy
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
initialize(policy p);

/// Adds a new contiguous memory region to the physical memory manager.
error
add_region(paddr_t region_base, size_t region_size);

/// Removes a contiguous region from the physical memory manager.
error
remove_region(paddr_t region_base, size_t region_size);

/// Allocates a region from the physical memory manager with the requested size and alignment.
error
alloc_aligned(size_t size, size_t alignment, paddr_t* ret);

/// Allocate a region from the physical memory manager with the requested size and alignment, if there's an error, it
/// will return nullptr instead.
paddr_t
alloc_aligned_noerr(size_t size, size_t alignment);

/// Allocates a region of memory with the requested size and base page size alignment.
error
alloc(size_t size, paddr_t* ret);

/// Allocates a region of memory with the requested size and base page size alignment, if there's an error, it will
/// return nullptr instead.
paddr_t
alloc_noerr(size_t size);

/// Frees a previously allocated region of memory, returning the allocated memory back to the physical memory manager.
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
struct fmt::printer<pmm::policy>
{
    static void print(putCharFunc fn, const pmm::policy& p)
    {
        switch (p) {
            case pmm::policy::FIRST_FIT:
                printer<str_view>::print(fn, "pmm::policy::FIRST_FIT");
                break;
            case pmm::policy::BEST_FIT:
                printer<str_view>::print(fn, "pmm::policy::BEST_FIT");
                break;
            case pmm::policy::WORST_FIT:
                printer<str_view>::print(fn, "pmm::policy::WORST_FIT");
                break;
            case pmm::policy::NEXT_FIT:
                printer<str_view>::print(fn, "pmm::policy::NEXT_FIT");
                break;
        }
    }
};

// class pmm
// {
// public:
//     enum policy
//     {
//     };

//     constexpr explicit pmm(enum policy p);

//     /// Adds a new contiguous memory region to the page allocator.
//     constexpr error add_region(paddr_t region_base, size_t region_size);

//     /// Removes a contiguous region from the page allocator.
//     constexpr error remove_region(paddr_t region_base, size_t region_size);

//     /// Allocates a region of memory with the requested size and alignment.
//     constexpr error alloc_aligned(size_t size, size_t alignment);

//     /// Frees a previously allocated region of memory.
//     constexpr error free(u8* ret);

//     /// Returns the total amount of memory managed by this allocator.
//     constexpr size_t total_memory();

//     /// Returns the total amount of free memory managed by this allocator.
//     constexpr size_t free_memory();

// private:
//     /// Individual chunks of free memory within a memory region.
//     struct memory_block
//     {
//         paddr_t base;
//         size_t length;
//         memory_block* next;
//     };

//     /// A memory region which has been added to the physical memory manager.
//     struct memory_region
//     {
// paddr_t base;
// size_t length;
// size_t free_bytes;
// memory_block* free_blocks;
//     };

//     static constexpr size_t REGION_COUNT = 16;

//     /// The allocation policy for this allocator.
//     policy m_policy;
//     /// Total amount of memory managed by this allocator.
//     size_t m_total_bytes;
//     /// Total amount of free memory managed by this allocator.
//     size_t m_free_bytes;
//     /// Number of regions in the list.
//     size_t m_region_count;
//     /// List of contiguous regions from which memory can be allocated.
//     memory_region m_regions[REGION_COUNT] = {};
//     /// Slab allocator for MemoryBlock structures.
//     slab_alloc<memory_block> m_block_allocator = {};
// };
