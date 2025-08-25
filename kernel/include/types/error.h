#pragma once

#include <fmt/printer.h>
#include <types/number.h>
#include <types/str_view.h>

enum class ErrorCode : u8
{
    SUCCESS,
    NOT_IMPLEMENTED,
    NULL_ARGUMENT,
    SLAB_REGION_TOO_SMALL,
    SLAB_BAD_ALIGN,

    LIMINE_REQUEST_ERROR,

    PMM_REGION_TOO_SMALL,
    PMM_REGION_LIST_FULL,
    PMM_REGION_MANAGED,
    PMM_REGION_NOT_MANAGED,
    PMM_BAD_ALIGN,
    PMM_OUT_OF_MEM,

    ELF_MAGIC_NUMBER,
    ELF_CLASS_32BIT,
    ELF_ENDIANNESS,
    ELF_WRONG_VERSION,
    ELF_NON_SYSVABI,
    ELF_WRONG_ABI_VERSION,
    ELF_UNSUPPORTED_TYPE,
    ELF_UNSUPPORTED_MACHINE,

    PAGING_UNALIGNED_ADDR,
    PAGING_ALLOC_FAILED,
    PAGING_MAP_EXISTS,

    DYN_ARR_REALLOC_FAILURE,
    DYN_ARR_ALLOC_FAILURE,

    DT_MAGIC_NUMBER,
    DT_NO_NODES,
    DT_REWRITE_FAILED,
    DT_ADDRESS_CELLS_TOO_LARGE,
    DT_SIZE_CELLS_TOO_LARGE,

    ERROR_CODE_GUARD_VALUE,
};

static str_view error_strings[] = {
    [static_cast<u8>(ErrorCode::SUCCESS)] = "SYS_OK: No error.",
    [static_cast<u8>(ErrorCode::NOT_IMPLEMENTED)] = "NOT_IMPLEMENTED: This function has not been implemented.",
    [static_cast<u8>(ErrorCode::NULL_ARGUMENT)] = "NULL_ARGUMENT: Pointer argument to function was NULL.",
    [static_cast<u8>(ErrorCode::SLAB_REGION_TOO_SMALL)] =
      "SLAB_REGION_TOO_SMALL: Region added to slab_alloc is too small to allocate a block from.",
    [static_cast<u8>(ErrorCode::SLAB_BAD_ALIGN)] =
      "SLAB_BAD_ALIGN: Region added to slab_alloc is not aligned properly.",

    [static_cast<u8>(ErrorCode::LIMINE_REQUEST_ERROR)] = "LIMINE_REQUEST_ERROR: Limine requests failed.",

    // [PMM_REGION_TOO_SMALL] = "PMM_REGION_TOO_SMALL: memory block allocator failed to
    // allocate.",
    [static_cast<u8>(ErrorCode::PMM_REGION_TOO_SMALL)] =
      "PMM_REGION_TOO_SMALL: Added a region that was smaller than system BASE_PAGE_SIZE.",
    [static_cast<u8>(ErrorCode::PMM_REGION_LIST_FULL)] =
      "PMM_REGION_LIST_FULL: Region list is full, can't add more regions.",
    [static_cast<u8>(ErrorCode::PMM_REGION_MANAGED)] =
      "PMM_REGION_MANAGED: Added a region that was already being managed.",
    [static_cast<u8>(ErrorCode::PMM_REGION_NOT_MANAGED)] =
      "PMM_REGION_NOT_MANAGED: Removed a region that was not being managed.",
    [static_cast<u8>(ErrorCode::PMM_BAD_ALIGN)] =
      "PMM_BAD_ALIGN: Alignment must be a power of two and at least system BASE_PAGE_SIZE.",
    [static_cast<u8>(ErrorCode::PMM_OUT_OF_MEM)] =
      "PMM_OUT_OF_MEM: There is not enough free memory to satisfy the allocation request.",

    [static_cast<u8>(ErrorCode::ELF_MAGIC_NUMBER)] =
      "ELF_MAGIC_NUMBER: Invalid magic number found in file header, file may not be an ELF file.",
    [static_cast<u8>(ErrorCode::ELF_CLASS_32BIT)] =
      "ELF_CLASS_32BIT: System only supports 64-bit ELF files, but a 32-bit ELF file was found.",
    [static_cast<u8>(ErrorCode::ELF_ENDIANNESS)] = "ELF_ENDIANNESS: System only supports little-endian ELF files.",
    [static_cast<u8>(ErrorCode::ELF_WRONG_VERSION)] =
      "ELF_WRONG_VERSION: ELF file is compiled with non-current ELF version.",
    [static_cast<u8>(ErrorCode::ELF_NON_SYSVABI)] =
      "ELF_NON_SYSVABI: ELF file is compiled with an ABI that isn't System-V.",
    [static_cast<u8>(ErrorCode::ELF_WRONG_ABI_VERSION)] =
      "ELF_WRONG_ABI_VERSION: Elf file was not compiled for System-V version 3.",
    [static_cast<u8>(ErrorCode::ELF_UNSUPPORTED_TYPE)] =
      "ELF_UNSUPPORTED_TYPE: Elf file type is not one-of: { Relocatable, Executable, SharedObject, "
      "CoreDump }.",
    [static_cast<u8>(ErrorCode::ELF_UNSUPPORTED_MACHINE)] =
      "ELF_UNSUPPORTED_MACHINE: Elf file was not compiled for RISC-V.",

    [static_cast<u8>(ErrorCode::PAGING_UNALIGNED_ADDR)] =
      "PAGING_UNALIGNED_ADDR: Attempted to install a page table with an unaligned vaddr_t or "
      "paddr_t.",
    [static_cast<u8>(ErrorCode::PAGING_ALLOC_FAILED)] =
      "PAGING_ALLOC_FAILED: Physical paging allocation for intermediate page table failed.",
    [static_cast<u8>(ErrorCode::PAGING_MAP_EXISTS)] =
      "PAGING_MAP_EXISTS: Attempted to install a mapping where one already exists.",

    [static_cast<u8>(ErrorCode::DYN_ARR_REALLOC_FAILURE)] = "DYN_ARR_REALLOC_FAILURE: Failed to grow a dynamic array.",
    [static_cast<u8>(ErrorCode::DYN_ARR_ALLOC_FAILURE)] =
      "DYN_ARR_ALLOC_FAILURE: Failed to allocate initial memory for dynamic array.",

    [static_cast<u8>(ErrorCode::DT_MAGIC_NUMBER)] =
      "DT_MAGIC_NUMBER: The device tree blob magic number is invalid. Expected 0xD00DFEED.",
    [static_cast<u8>(ErrorCode::DT_NO_NODES)] = "DT_NO_NODES: The parsed device tree blob was empty.",
    [static_cast<u8>(ErrorCode::DT_REWRITE_FAILED)] =
      "DT_REWRITE_FAILED: Failed to rewrite device tree properties, either due to an unsupported "
      "property type or a malformed DTB.",
    [static_cast<u8>(ErrorCode::DT_ADDRESS_CELLS_TOO_LARGE)] =
      "DT_ADDRESS_CELLS_TOO_LARGE: Failed to parse device tree, #address-cells property encountered with a value "
      "larger than 3.",
    [static_cast<u8>(ErrorCode::DT_SIZE_CELLS_TOO_LARGE)] =
      "DT_SIZE_CELLS_TOO_LARGE: Failed to parse device tree, #size-cells property encountered with a value larger "
      "than 2.",
};

static_assert(sizeof(ErrorCode) == 1, "ErrorCode must be fit in a single byte.");
static_assert(sizeof(error_strings) / sizeof(error_strings[0]) == static_cast<u8>(ErrorCode::ERROR_CODE_GUARD_VALUE),
              "Error strings array must match the number of ErrorCode values.");

class error
{
public:
    /// Creates an empty error stack.
    constexpr error()
      : m_stack(0)
    {
    }

    // Creates an error from an ErrorCode.
    constexpr error(enum ErrorCode code)
      : m_stack(static_cast<u8>(code))
    {
    }

    /// Pushes an error code onto the stack. Erases the bottom-most element if the stack is full.
    error push(ErrorCode code)
    {
        m_stack <<= sizeof(ErrorCode);
        m_stack |= static_cast<u8>(code);
        return error(m_stack);
    }

    /// Pops the top error code from the stack.
    error pop() { return error(m_stack >> sizeof(ErrorCode)); }

    /// Returns the top error value from the stack without removing it.
    ErrorCode top() const { return static_cast<ErrorCode>(m_stack & 0xFF); }

    /// Returns true if the top error code is not an error.
    bool is_ok() const { return top() == ErrorCode::SUCCESS; }

    /// Returns true if the top error code is an error.
    bool is_err() const { return !is_ok(); }

    /// Returns the string representation of the top error code.
    str_view str() const { return error_strings[static_cast<u8>(top())]; }

private:
    /// Implicit creator for error from a u64 stack value.
    constexpr explicit error(u64 stack)
      : m_stack(stack)
    {
    }

    u64 m_stack;
};

template<>
struct fmt::printer<error>
{
    static void print(putCharFunc fn, error err) { print_with_func(fn, err.str()); }
};

template<>
struct fmt::printer<ErrorCode>
{
    static void print(putCharFunc fn, ErrorCode err) { print_with_func(fn, error(err).str()); }
};