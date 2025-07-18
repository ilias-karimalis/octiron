#pragma once

#include <types/number.hpp>
#include <types/str_view.hpp>

enum ErrorCode : u8
{
    SUCCESS,
    NOT_IMPLEMENTED,
    NULL_ARGUMENT,
    SLAB_REGION_TOO_SMALL,
    SLAB_BAD_ALIGN,

    PMM_REGION_TOO_SMALL,
    PMM_REGION_LIST_FULL,
    PMM_REGION_MANAGED,
    PMM_REGION_NOT_MANAGED,
    PMM_BAD_ALIGN,
    PMM_OUT_OF_MEM,

    ERROR_CODE_GUARD_VALUE,
};

static str_view error_strings[] = {
    [SUCCESS] = "SYS_OK: No error.",
    [NOT_IMPLEMENTED] = "NOT_IMPLEMENTED: This function has not been implemented.",
    [NULL_ARGUMENT] = "NULL_ARGUMENT: Pointer argument to function was NULL.",
    [SLAB_REGION_TOO_SMALL] =
      "SLAB_REGION_TOO_SMALL: Region added to slab_alloc is too small to allocate a block from.",
    [SLAB_BAD_ALIGN] = "SLAB_BAD_ALIGN: Region added to slab_alloc is not aligned properly.",

    // [PMM_REGION_TOO_SMALL] = "PMM_REGION_TOO_SMALL: memory block allocator failed to allocate.",
    [PMM_REGION_TOO_SMALL] = "PMM_REGION_TOO_SMALL: Added a region that was smaller than system BASE_PAGE_SIZE.",
    [PMM_REGION_LIST_FULL] = "PMM_REGION_LIST_FULL: Region list is full, can't add more regions.",
    [PMM_REGION_MANAGED] = "PMM_REGION_MANAGED: Added a region that was already being managed.",
    [PMM_REGION_NOT_MANAGED] = "PMM_REGION_NOT_MANAGED: Removed a region that was not being managed.",
    [PMM_BAD_ALIGN] = "PMM_BAD_ALIGN: Alignment must be a power of two and at least system BASE_PAGE_SIZE.",
    [PMM_OUT_OF_MEM] = "PMM_OUT_OF_MEM: There is not enough free memory to satisfy the allocation request.",
};

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
      : m_stack(code)
    {
    }

    /// Pushes an error code onto the stack. Erases the bottom-most element if the stack is full.
    error push(ErrorCode code)
    {
        m_stack <<= sizeof(ErrorCode);
        m_stack |= code;
        return error(m_stack);
    }

    /// Pops the top error code from the stack.
    error pop() { return error(m_stack >> sizeof(ErrorCode)); }

    /// Returns the top error value from the stack without removing it.
    ErrorCode top() const { return static_cast<ErrorCode>(m_stack & 0xFF); }

    /// Returns true if the top error code is not an error.
    bool is_ok() const { return top() == SUCCESS; }

    /// Returns true if the top error code is an error.
    bool is_err() const { return !is_ok(); }

    /// Returns the string representation of the top error code.
    str_view str() const { return error_strings[top()]; }

private:
    /// Implicit creator for error from a u64 stack value.
    constexpr explicit error(u64 stack)
      : m_stack(stack)
    {
    }

    u64 m_stack;
};

static_assert(sizeof(ErrorCode) == 1, "ErrorCode must be fit in a single byte.");
static_assert(sizeof(error_strings) / sizeof(error_strings[0]) == ERROR_CODE_GUARD_VALUE,
              "Error strings array must match the number of ErrorCode values.");
