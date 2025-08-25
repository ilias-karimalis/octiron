// Kernel Virtual Address Space management functions.
#pragma once

#include <riscv/sv39.h>

struct kvspace
{
    struct region
    {
        size_t m_size;
        size_t m_alignment;
    };
};

// What kind of functions do we need?
// Reserve/Allocate region
size_t
reserve_region(size_t size, size_t alignment);

// Map pre-allocated region
size_t
map_region(size_t region_base, size_t size);

// Reserve_Map region
size_t
reserve_and_map_region(size_t size, size_t alignment);

// Reallocates a memory region using
void*
reallocate(void* ptr, size_t new_size);