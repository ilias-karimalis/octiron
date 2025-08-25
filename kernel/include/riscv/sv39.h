#pragma once

#include <cstddef>
#include <types/error.h>
#include <types/number.h>
#include <types/static_array.h>

namespace riscv::sv39 {
constexpr size_t PAGE_SIZE = 0x1000;
constexpr size_t MEGAPAGE_SIZE = 0x200000;
constexpr size_t GIGAPAGE_SIZE = 0x40000000;

enum TableEntryFlags : u64
{
    /// Valid bit
    TEF_VALID = 0b00000001,
    /// Read bit
    TEF_READ = 0b00000010,
    /// Write bit
    TEF_WRITE = 0b00000100,
    /// Execute bit
    TEF_EXECUTE = 0b00001000,
    /// User bit
    TEF_USER = 0b00010000,
    /// Global bit
    TEF_GLOBAL = 0b00100000,
    /// Accessed bit
    TEF_ACCESSED = 0b01000000,
    /// Dirty bit
    TEF_DIRTY = 0b10000000,
};

/// An entry of a riscv sv39 page table.
struct table_entry
{
public:
    /// Default constructs a nulled-out table_entry.
    constexpr table_entry() noexcept
      : m_entry(0)
    {
    }
    /// Constructs a table_entry with a paddr_t and the given flags.
    constexpr table_entry(paddr_t page_address, u64 flags)
      : m_entry((page_address >> 2) | flags)
    {
    }
    /// Returns the physical address to the next level page_table or normal PAGE_SIZE.
    constexpr paddr_t get_address() const { return ((m_entry >> 10) << 12); }
    /// Returns the physical address of a MEGA_PAGE that's being pointed to.
    constexpr paddr_t get_megapage_addr() const { return ((m_entry >> 10) << 21); }
    /// Returns the physical address of a GIGA_PAGE that's being pointed to.
    constexpr paddr_t get_gigapage_addr() const { return ((m_entry >> 10) << 30); }
    /// Returns true if this entry is valid.
    constexpr bool is_valid() const { return 0 != (m_entry & TEF_VALID); }
    /// Returns true if the entry points to a readable page.
    constexpr bool is_readable() const { return 0 != (m_entry & TEF_READ); }
    /// Returns true if the entry points to a writable page.
    constexpr bool is_writable() const { return 0 != (m_entry & TEF_WRITE); }
    /// Returns true if the entry points to an executable page.
    constexpr bool is_executable() const { return 0 != (m_entry & TEF_EXECUTE); }
    /// Returns true if the entry points to a leaf node
    constexpr bool is_leaf() const { return is_readable() || is_writable() || is_executable(); }

private:
    u64 m_entry;
};

constexpr table_entry NULL_TABLE_ENTRY = table_entry(0, 0);
constexpr size_t TABLE_ENTRY_COUNT = 512;

/// The page table is effectively page aligned set of 512 table_entries.
using page_table = static_array<table_entry, TABLE_ENTRY_COUNT>;

/// Mapos a page into the virtual address space, given a root page table. If any level of the page
/// table doesn't exist, then it is created.
error
map_small_page(page_table* root, vaddr_t va, paddr_t pa, u64 flags);

/// Walks a page table translating a vaddr_t to a paddr_t if the mapping is present, if the mapping
/// is not present it returns ((paddr_t)-1).
paddr_t
virt_to_phys(page_table* root, vaddr_t va);

page_table*
current_page_table();

} // namespace riscv::sv39
