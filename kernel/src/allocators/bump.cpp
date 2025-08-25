#include <allocators/bump.h>
#include <fmt/assert.h>
#include <limine/platform_info.h>
#include <pmm.h>
#include <riscv/sv39.h>
#include <types/number.h>

bump_alloc::~bump_alloc()
{
    for (struct region* curr = m_region_list; curr != nullptr; curr = curr->next) {
        paddr_t pa = limine::hhdm_virt_to_phys(curr);
        error err = pmm::free(pa);
        assert_err(err);
    }
    m_region_list = nullptr;
    m_current_region = nullptr;
}

error
bump_alloc::grow()
{
    error err = error();
    paddr_t pa;
    if ((err = pmm::alloc(riscv::sv39::PAGE_SIZE, &pa)).is_err()) {
        return err;
    }
    m_region_list = (struct region*)limine::hhdm_phys_to_virt(pa);
    m_region_list->end = (u8*)m_region_list + riscv::sv39::PAGE_SIZE;
    m_region_list->curr = (u8*)m_region_list + sizeof(struct region);
    m_region_list->next = nullptr;
    m_current_region = m_region_list;
    return err;
}

void*
bump_alloc::alloc(size_t size)
{
    if (m_current_region == nullptr || size == 0) {
        return nullptr;
    }

    if (m_current_region == nullptr || m_region_list == nullptr) {
        error err = grow();
        assert_err(err);
    }

    if (m_current_region->end < m_current_region->curr + size) {
        size_t difference = m_current_region->curr + size - m_current_region->end;
        size_t region_size = align_up(difference, riscv::sv39::PAGE_SIZE) + riscv::sv39::PAGE_SIZE;
        paddr_t pa;
        error err = pmm::alloc(region_size, &pa);
        assert_err(err);
        struct region* new_region = (struct region*)limine::hhdm_phys_to_virt(pa);
        m_current_region->next = new_region;
        new_region->end = (u8*)new_region + region_size;
        new_region->curr = (u8*)new_region + sizeof(struct region);
        new_region->next = nullptr;
        m_current_region = new_region;
    }

    void* allocation = (void*)m_current_region->curr;
    m_current_region->curr += size;
    return allocation;
}

void*
bump_alloc::alloc_aligned(size_t size, size_t alignment)
{
    if (size == 0 || alignment == 0) {
        return nullptr;
    }

    if (m_current_region == nullptr || m_region_list == nullptr) {
        error err = grow();
        assert_err(err);
    }

    u8* aligned_curr = align_up(m_current_region->curr, alignment);
    if (m_current_region->end < aligned_curr + size) {
        size_t difference = aligned_curr + size - m_current_region->end;
        size_t region_size = align_up(difference, riscv::sv39::PAGE_SIZE) + riscv::sv39::PAGE_SIZE;
        paddr_t pa;
        error err = pmm::alloc(region_size, &pa);
        assert_err(err);
        struct region* new_region = (struct region*)limine::hhdm_phys_to_virt(pa);
        m_current_region->next = new_region;
        new_region->end = (u8*)new_region + region_size;
        new_region->curr = (u8*)new_region + sizeof(struct region);
        new_region->next = nullptr;
        m_current_region = new_region;
    }
    m_current_region->curr = align_up(m_current_region->curr, alignment);
    void* allocation = (void*)m_current_region->curr;
    m_current_region->curr += size;
    return allocation;
}

bump_alloc::bump_alloc(bump_alloc&& other) noexcept { todo(); }

bump_alloc&
bump_alloc::operator=(bump_alloc&& other) noexcept
{
    todo();
}