/// A simple type generic slab allocator.
#pragma once

#include "memory.hpp"
#include <fmt/assert.hpp>
#include <types/error.hpp>

template<typename T, bool ZeroOut = true>
class slab_alloc
{
public:
    /// Creates an allocator which allocates objects of type `T`.
    constexpr slab_alloc() = default;

    /// Creates an allocator at compile time with a buffer
    template<size_t N>
    consteval slab_alloc(u8 (&buffer)[N])
    {
        // static_assert(buffer != nullptr);
        // static_assert(reinterpret_cast<std::uintptr_t>(buffer) % region_align() == 0);
        // static_assert(N >= s_MINIMAL_REGION_SZ);
        this->grow(buffer, N);
    }

    /// Adds the `buffer` of length `buffer_size` to the allocator.
    constexpr void grow(void* buffer, size_t buffer_size)
    {
        assert(buffer != nullptr);
        assert(reinterpret_cast<size_t>(buffer) % region_align() == 0);
        assert(buffer_size >= s_MINIMAL_REGION_SZ);
        u8* ptr = static_cast<u8*>(buffer);

        // Set up the Region structure
        slab_region* new_region = reinterpret_cast<slab_region*>(ptr);
        buffer_size -= sizeof(slab_region);
        ptr += align_up(sizeof(slab_region), s_BLOCK_ALIGN);

        // Calculate the number of blocks in the buffer
        new_region->m_free = new_region->m_total = buffer_size / s_BLOCK_SZ;
        m_total += new_region->m_total;
        m_free += new_region->m_free;

        // Enque the blocks in the region free list
        slab_block* block = new_region->m_blocks = reinterpret_cast<slab_block*>(ptr);
        for (size_t i = 0; i < new_region->m_total; i++) {
            ptr += s_BLOCK_SZ;
            block->hdr.m_next = reinterpret_cast<slab_block*>(ptr);
            block = reinterpret_cast<slab_block*>(ptr);
        }
        block->hdr.m_next = nullptr;

        // Add the region to the allocator
        new_region->m_next = m_regions;
        m_regions = new_region;
    }

    /// Allocates an object of type `T`.
    constexpr T* alloc()
    {
        if (m_free == 0)
            return nullptr;

        // Find the first non-empty region
        slab_region* region = m_regions;
        while (region != nullptr && region->m_free == 0)
            region = region->m_next;
        if (region == nullptr)
            return nullptr;

        // Dequeue the first block from the region free list
        slab_block* b = region->m_blocks;
        region->m_blocks = b->hdr.m_next;
        --region->m_free;
        m_free--;

        if constexpr (ZeroOut) {
            mem::fill(b, 0, s_BLOCK_SZ);
        }

        return &b->obj;
    }

    /// Returns an object of type `T` to the allocator.
    constexpr error free(T* obj)
    {
        (void)obj;
        return ErrorCode::NOT_IMPLEMENTED;
    }

    /// The count of free blocks managed by the allocator.
    constexpr size_t free_count() { return m_free; }

    /// Calculates the ammount of memory needed for a region with `n` elements.
    static consteval size_t region_size(size_t n)
    {
        return align_up(sizeof(slab_region), s_BLOCK_ALIGN) + n * s_BLOCK_SZ;
    }

    /// Calculates the correct alignment for a region.
    static consteval size_t region_align() { return alignof(slab_region); }

private:
    union slab_block
    {
        struct slab_block_hdr
        {
            slab_block* m_next;
        } hdr;

        T obj;
    };

    struct slab_region
    {
        u64 m_total;
        u64 m_free;
        slab_block* m_blocks;
        slab_region* m_next;
    };

    static constexpr size_t s_BLOCK_SZ = sizeof(slab_block);
    static constexpr size_t s_BLOCK_ALIGN = alignof(slab_block);
    static constexpr size_t s_MINIMAL_REGION_SZ = region_size(1);

    slab_region* m_regions = nullptr;
    u64 m_total = 0;
    u64 m_free = 0;
};

namespace slab_details {
};
