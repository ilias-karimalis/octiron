#include <colls/slab_alloc.hpp>
#include <cstddef>
#include <cstring>
#include <fmt/assert.hpp>
#include <fmt/print.hpp>
#include <memory.hpp>
#include <pmm.hpp>
#include <riscv/sv39.hpp>
#include <types/error.hpp>
#include <types/number.hpp>

namespace pmm {

/// Individual chunks of free memory within a memory region.
struct memory_block
{
    paddr_t base;
    size_t length;
    memory_block* next;
};

// Buffer used to initialize the block allocator.
constexpr size_t INITIAL_MEM_BLOCK_COUNT = 64;
constexpr size_t BLOCK_BUF_ALIGN = slab_alloc<memory_block>::region_align();
constexpr size_t BLOCK_BUF_SIZE = slab_alloc<memory_block>::region_size(INITIAL_MEM_BLOCK_COUNT);
alignas(BLOCK_BUF_ALIGN) u8 BLOCK_BUF[BLOCK_BUF_SIZE] = { 0 };

/// A contiguous chunk of memory which has been added to the physical memory manager.
struct memory_region
{
    paddr_t base;
    size_t length;
    size_t free_bytes;
    memory_block* free_blocks;
};
static constexpr size_t REGION_COUNT = 16;

/// The allocation policy currently in use.
policy pol = policy::FIRST_FIT;
/// Total amount of memory managed by this physical memory manager.
size_t total_bytes = 0;
/// Total amount of free memory managed by this physical memory manager
size_t free_bytes = 0;
/// Number of regions in the region list
size_t region_count = 0;
/// List of contiguous regions from which memory can be allocated
memory_region regions[REGION_COUNT] = { 0 };
/// Slab allocator for memory_block structs.
slab_alloc<memory_block> block_allocator = {};

void
initialize(policy p)
{
    pol = p;
    block_allocator.grow(reinterpret_cast<void*>(BLOCK_BUF), BLOCK_BUF_SIZE);
}

error
add_region(paddr_t region_base, size_t region_size)
{
    if (region_base == 0) {
        return ErrorCode::NULL_ARGUMENT;
    }
    if (region_count >= REGION_COUNT) {
        return ErrorCode::PMM_REGION_LIST_FULL;
    }

    // Check that the aligned base and size region is at least BASE_PAGE_SIZE
    size_t aligned_base = align_up(region_base, riscv::sv39::PAGE_SIZE);
    size_t aligned_size = align_down(region_size - (aligned_base - region_base), riscv::sv39::PAGE_SIZE);
    bool ALIGNED_REGION_FITS = aligned_base + aligned_size <= region_base + region_size;
    bool NEW_SIZE_NON_ZERO = aligned_size >= riscv::sv39::PAGE_SIZE;
    if (!ALIGNED_REGION_FITS || !NEW_SIZE_NON_ZERO) {
        return ErrorCode::PMM_REGION_TOO_SMALL;
    }

    // Check if this region overlaps with a managed region.
    for (size_t i = 0; i < region_count; i++) {
        bool LOWER_BOUND = regions[i].base <= aligned_base;
        bool UPPER_BOUND = aligned_base <= regions[i].base + regions[i].length;
        if (LOWER_BOUND && UPPER_BOUND) {
            return ErrorCode::PMM_REGION_MANAGED;
        }
    }

    // Create and instantiate the region struct
    struct memory_block* free_block = block_allocator.alloc();
    assert(free_block != nullptr);
    regions[region_count].base = aligned_base;
    regions[region_count].length = aligned_size;
    regions[region_count].free_bytes = aligned_size;
    regions[region_count].free_blocks = block_allocator.alloc();
    assert(free_block != nullptr);
    regions[region_count].free_blocks->base = aligned_base;
    regions[region_count].free_blocks->length = aligned_size;
    regions[region_count].free_blocks->next = nullptr;
    region_count++;

    total_bytes += aligned_size;
    free_bytes += aligned_size;
    return ErrorCode::SUCCESS;
}

error
remove_region(paddr_t region_base, size_t region_size)
{
    return ErrorCode::NOT_IMPLEMENTED;
}

error
alloc_aligned(size_t size, size_t alignment, paddr_t* ret)
{
    size = align_up(size, riscv::sv39::PAGE_SIZE);
    if (ret == nullptr) {
        return ErrorCode::NULL_ARGUMENT;
    }
    if (alignment < riscv::sv39::PAGE_SIZE || (alignment & (alignment - 1)) != 0) {
        *ret = 0;
        return ErrorCode::PMM_BAD_ALIGN;
    }
    if (free_bytes < size) {
        *ret = 0;
        return ErrorCode::PMM_OUT_OF_MEM;
    }

    // Check if we need to fill up the memory_block allocator
    if (block_allocator.free_count() < 16) {
        todo("memory_block slab allocator is out of memory!!! Need to implement refilling!!!");
    }

    if (pol != policy::FIRST_FIT) {
        todo("policy: ", pol, " is not implemented");
    }

    for (size_t i = 0; i < region_count; i++) {
        memory_region& region = regions[i];
        if (region.free_bytes < size) {
            continue;
        }

        memory_block* curr = region.free_blocks;
        memory_block* prev = nullptr;
        while (curr != nullptr) {
            size_t curr_base = curr->base;
            size_t curr_len = curr->length;
            size_t aligned_base = align_up(curr_base, alignment);
            if (curr_base + curr_len < aligned_base + size) [[likely]] {
                prev = curr;
                curr = curr->next;
            }

            size_t offset = aligned_base - curr_base;
            bool EXISTS_PRECEEDING = curr_base != aligned_base;
            bool EXISTS_POSTCEEDING = curr_base + curr_len > aligned_base + size;
            if (EXISTS_PRECEEDING && EXISTS_POSTCEEDING) {
                // Need a new block
                curr->length = offset;
                memory_block* extra = block_allocator.alloc();
                assert(extra != nullptr);
                extra->base = aligned_base + size;
                extra->length = curr_base + curr_len - (aligned_base + size);
                extra->next = curr->next;
            } else if (EXISTS_PRECEEDING) {
                curr->length = offset;
            } else if (EXISTS_POSTCEEDING) {
                curr->base = aligned_base + size;
                curr->length = curr_base + curr_len - (aligned_base + size);
            } else if (prev == nullptr) {
                region.free_blocks = curr->next;
            } else {
                prev->next = curr->next;
                error err = block_allocator.free(curr);
                assert(err.is_ok());
            }

            region.free_bytes -= size;
            free_bytes -= size;
            *ret = aligned_base;

            // Zero out the page being given out.
            mem::fill(reinterpret_cast<void*>(aligned_base), 0, size);
            return SUCCESS;
        }
    }

    *ret = 0;
    return PMM_OUT_OF_MEM;
}

paddr_t
alloc_aligned_noerr(size_t size, size_t alignment)
{
    paddr_t ret;
    error err = alloc_aligned(size, alignment, &ret);
    assert(err.is_ok());
    return ret;
}

error
alloc(size_t size, paddr_t* ret)
{
    return alloc_aligned(size, riscv::sv39::PAGE_SIZE, ret);
}

paddr_t
alloc_noerr(size_t size)
{
    return alloc_aligned_noerr(size, riscv::sv39::PAGE_SIZE);
}

error
free(paddr_t ret)
{
    todo("Need to implement the pmm free function.");
    return NOT_IMPLEMENTED;
}

size_t
total_memory()
{
    return total_bytes;
}

size_t
free_memory()
{
    return free_bytes;
}

}
