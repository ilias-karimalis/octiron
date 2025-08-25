#pragma once

#include <limine/limine.h>
#include <types/error.h>
#include <types/number.h>

namespace limine {

struct platform_info
{
    /// Framebuffer
    u64 framebuffer_count;
    struct limine_framebuffer** framebuffers;

    /// Paging Mode
    u64 mode;

    /// Memory Map
    u64 memmap_count;
    struct limine_memmap_entry* memmap;

    /// Device Tree Blob
    void* device_tree_blob;

    /// Higher Half Direct Mapping Base
    u64 hhdm_base;
};

const struct platform_info*
parse_platform_info();

void*
hhdm_phys_to_virt(paddr_t pa);

/// Note that the result will only be valiid if the void* is from the hhdm region.
paddr_t
hhdm_virt_to_phys(void* ptr);

} // namespace limine