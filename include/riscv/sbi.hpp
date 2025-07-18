#pragma once

#include <types/number.hpp>

namespace riscv::sbi {

struct retval
{
    i64 error;
    i64 value;
};

// enum error : i64
// {
//     SBI_SUCCESS = 0,
//     SBI_ERR_FAILED = -1,
//     SBI_ERR_NOT_SUPPORTED = -2,
//     SBI_ERR_INVALID_PARAM = -3,
//     SBI_ERR_DENIED = -4,
//     SBI_ERR_INVALID_ADDRESS = -5,
//     SBI_ERR_ALREADY_AVAILABLE = -6,
//     SBI_ERR_ALREADY_STARTED = -7,
//     SBI_ERR_ALREADY_STOPPED = -8,
//     SBI_ERR_NO_SHMEM = -9
// };

/// Write data present in `ch` to debug console. Returns and sbi error code,
i64
console_putchar(char ch);

/// Read a byte from debug console. Returns the byte on success or -1 for failure.
i64
console_getchar();

/// Prints a string to the debug console.
retval
console_print_string(const char* str, size_t length);

namespace detail {

inline retval
ecall(i64 eid, i64 fid, i64 arg0 = 0, i64 arg1 = 0, i64 arg2 = 0, i64 arg3 = 0, i64 arg4 = 0, i64 arg5 = 0)
{
    retval result;
    register i64 a0 asm("a0") = arg0;
    register i64 a1 asm("a1") = arg1;
    register i64 a2 asm("a2") = arg2;
    register i64 a3 asm("a3") = arg3;
    register i64 a4 asm("a4") = arg4;
    register i64 a5 asm("a5") = arg5;
    register i64 a6 asm("a6") = fid;
    register i64 a7 asm("a7") = eid;
    asm volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) : "memory");
    result.error = a0;
    result.value = a1;
    return result;
}
}
}
