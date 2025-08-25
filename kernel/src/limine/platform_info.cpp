#include "limine/limine.h"
#include "types/number.h"
#include <fmt/assert.h>
#include <limine/platform_info.h>
#include <types/error.h>

namespace limine {

#define LIMINE_REQ __attribute__((used, section(".limine_requests")))
#define LIMINE_START __attribute__((used, section(".limine_requests_start")))
#define LIMINE_END __attribute__((used, section(".limine_requests_end")))

/// Start Marker for the Limine Request Section
LIMINE_START volatile LIMINE_REQUESTS_START_MARKER;

LIMINE_REQ volatile LIMINE_BASE_REVISION(0);

/// Frame buffer request
LIMINE_REQ volatile limine_framebuffer_request framebuffer_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .response = nullptr,
};

/// Paging Mode request
LIMINE_REQ volatile limine_paging_mode_request paging_mode_req = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .response = nullptr,
    .mode = LIMINE_PAGING_MODE_RISCV_SV39
};

/// Memory Map request
LIMINE_REQ volatile limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .response = nullptr,
};

/// Device Tree Blob request
LIMINE_REQ volatile limine_dtb_request dtb_req = {
    .id = LIMINE_DTB_REQUEST,
    .response = nullptr,
};

/// HigherHalf Direct Mapping request
LIMINE_REQ volatile limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
    .response = nullptr,
};

/// End Marker for the Limine Request Section
LIMINE_END volatile LIMINE_REQUESTS_END_MARKER;

struct platform_info pinfo = {};

const struct platform_info*
parse_platform_info()
{
    assert(LIMINE_BASE_REVISION_SUPPORTED != false, error(ErrorCode::LIMINE_REQUEST_ERROR));
    assert(framebuffer_req.response != nullptr, error(ErrorCode::LIMINE_REQUEST_ERROR));
    assert(paging_mode_req.response != nullptr, error(ErrorCode::LIMINE_REQUEST_ERROR));
    assert(memmap_req.response != nullptr, error(ErrorCode::LIMINE_REQUEST_ERROR));
    assert(dtb_req.response != nullptr, error(ErrorCode::LIMINE_REQUEST_ERROR));
    assert(hhdm_req.response != nullptr, error(ErrorCode::LIMINE_REQUEST_ERROR));

    pinfo = {
        .framebuffer_count = framebuffer_req.response->framebuffer_count,
        .framebuffers = framebuffer_req.response->framebuffers,
        .mode = paging_mode_req.response->mode,
        .memmap_count = memmap_req.response->entry_count,
        .memmap = *memmap_req.response->entries,
        .device_tree_blob = dtb_req.response->dtb_ptr,
        .hhdm_base = hhdm_req.response->offset,
    };
    return &pinfo;
}

void*
hhdm_phys_to_virt(paddr_t pa)
{
    return (void*)(pa + pinfo.hhdm_base);
}

paddr_t
hhdm_virt_to_phys(void* ptr)
{
    return (paddr_t)ptr - pinfo.hhdm_base;
}

} // namespace limine