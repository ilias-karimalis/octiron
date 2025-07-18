/// Implementation of Read/Write functions for the riscv Control and Status Registers

#include <types/number.hpp>
#include <types/str_view.hpp>

#ifndef __GXX_CONSTEXPR_ASM__
#error "Requires __GXX_CONSTEXPR_ASM__"
#endif 

namespace riscv { 

enum class csr {
    medeleg,
    mideleg,
    mie,
    mstatus,
    mtvec,
    mip,
    mcounteren,
    pmpaddr0,
    pmpcfg0,
    sstatus,
    stvec,
    sip,
    sie,
    scounteren,
    sscratch,
    sepc,
    scause,
    stval,
    senvcfg,
    satp
};

consteval str_view csrw_asm_string_view(csr reg) {
    switch (reg) {
    case csr::medeleg: return "csrw medeleg, %0";
    case csr::mideleg: return "csrw mideleg, %0";
    case csr::mie: return "csrw mie, %0";
    case csr::mstatus: return "csrw mstatus, %0";
    case csr::mtvec: return "csrw mtvec, %0";
    case csr::mip: return "csrw mip, %0";
    case csr::mcounteren: return "csrw mcounteren, %0";
    case csr::pmpaddr0: return "csrw pmpaddr0, %0";
    case csr::pmpcfg0: return "csrw pmpcfg0, %0";
    case csr::sstatus: return "csrw sstatus, %0";
    case csr::stvec: return "csrw stvec, %0";
    case csr::sip: return "csrw sip, %0";
    case csr::sie: return "csrw sie, %0";
    case csr::scounteren: return "csrw scounteren, %0";
    case csr::sscratch: return "csrw sscratch, %0";
    case csr::sepc: return "csrw sepc, %0";
    case csr::scause: return "csrw scause, %0";
    case csr::stval: return "csrw stval, %0";
    case csr::senvcfg: return "csrw senvcfg, %0";
    case csr::satp: return "csrw satp, %0";
    }
}

consteval str_view csrr_asm_string_view(csr reg) {
    switch (reg) {
    case csr::medeleg: return "csrr %0, medeleg";
    case csr::mideleg: return "csrr %0, mideleg";
    case csr::mie: return "csrr %0, mie";
    case csr::mstatus: return "csrr %0, mstatus";
    case csr::mtvec: return "csrr %0, mtvec";
    case csr::mip: return "csrr %0, mip";
    case csr::mcounteren: return "csrr %0, mcounteren";
    case csr::pmpaddr0: return "csrr %0, pmpaddr0";
    case csr::pmpcfg0: return "csrr %0, pmpcfg0";
    case csr::sstatus: return "csrr %0, sstatus";
    case csr::stvec: return "csrr %0, stvec";
    case csr::sip: return "csrr %0, sip";
    case csr::sie: return "csrr %0, sie";
    case csr::scounteren: return "csrr %0, scounteren";
    case csr::sscratch: return "csrr %0, sscratch";
    case csr::sepc: return "csrr %0, sepc";
    case csr::scause: return "csrr %0, scause";
    case csr::stval: return "csrr %0, stval";
    case csr::senvcfg: return "csrr %0, senvcfg";
    case csr::satp: return "csrr %0, satp";
    }
}

/// Write a 64-bit value to a Control and Status Register.
template <csr reg>
inline void csrw(u64 value)
{
    asm volatile((csrw_asm_string_view(reg)) : : "r"(value) : "memory");
}

/// Reads a 64-bit value from a Control and Status Register and returns it.
template <csr reg>
inline u64 csrr()
{
    u64 value;
    asm volatile((csrr_asm_string_view(reg)) : "=r"(value) : : "memory");
    return value;
}

};