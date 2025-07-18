#pragma once

namespace riscv { 

inline void sfence_vma(void)
{
	asm volatile("sfence.vma" : : : "memory");
}

}