#include <pmm.h>
#include <riscv/csr.h>
#include <riscv/sv39.h>
#include <types/error.h>
#include <types/number.h>

namespace riscv::sv39 {

error
map_small_page(page_table* root, vaddr_t va, paddr_t pa, u64 flags)
{
    error err = ErrorCode::SUCCESS;
    if (!is_aligned(va, alignof(page_table)) || !is_aligned(pa, alignof(page_table))) {
        return ErrorCode::PAGING_UNALIGNED_ADDR;
    }

    u64 vpn[] = {
        (va >> 12) & 0x1FF, // Level 0 index
        (va >> 21) & 0x1FF, // Level 1 index
        (va >> 30) & 0x1FF  // Level 2 index
    };

    page_table* l2_pt = root;
    table_entry& l2_entry = (*l2_pt)[vpn[2]];
    if (!l2_entry.is_valid()) {
        paddr_t page;
        err = pmm::alloc(PAGE_SIZE, &page);
        if (err.is_err())
            return err.push(ErrorCode::PAGING_ALLOC_FAILED);
        l2_entry = table_entry(page, TableEntryFlags::TEF_VALID);
    }

    page_table* l1_pt = reinterpret_cast<page_table*>(l2_entry.get_address());
    table_entry& l1_entry = (*l1_pt)[vpn[1]];
    if (!l1_entry.is_valid()) {
        paddr_t page;
        err = pmm::alloc(PAGE_SIZE, &page);
        if (err.is_err())
            return err.push(ErrorCode::PAGING_ALLOC_FAILED);
        l1_entry = table_entry(page, TableEntryFlags::TEF_VALID);
    }

    page_table* l0_pt = reinterpret_cast<page_table*>(l1_entry.get_address());
    table_entry& l0_entry = (*l0_pt)[vpn[0]];
    if (l0_entry.is_valid())
        return ErrorCode::PAGING_MAP_EXISTS;
    l0_entry = table_entry(pa, flags | TEF_VALID);

    return err;
}

paddr_t
virt_to_phys(page_table* root, vaddr_t va)
{
    u64 vpn[] = {
        (va >> 12) & 0x1FF, // Level 0 index
        (va >> 21) & 0x1FF, // Level 1 index
        (va >> 30) & 0x1FF  // Level 2 index
    };
    paddr_t page_offset = va & PAGE_SIZE;

    page_table* table = root;
    for (int level = 2; level >= 0; level--) {
        table_entry& pte = (*table)[vpn[level]];
        if (!pte.is_valid())
            return static_cast<paddr_t>(-1);
        if (pte.is_leaf()) {
            paddr_t pa = 0;
            switch (level) {
                case 2:
                    pa = pte.get_gigapage_addr() | (vpn[1] << 21) | (vpn[0] << 12);
                    break;
                case 1:
                    pa = pte.get_megapage_addr() || (vpn[0] << 12);
                    break;
                default:
                    pa = pte.get_address();
                    break;
            }
            return pa | page_offset;
        }
        table = reinterpret_cast<page_table*>(pte.get_address());
    }
    __builtin_unreachable();
}

page_table*
current_page_table()
{
    u64 satp = csrr<csr::satp>();
    u64 ppn = satp & 0x0FFFFFFFFFFF;
    return reinterpret_cast<page_table*>()
}
}
