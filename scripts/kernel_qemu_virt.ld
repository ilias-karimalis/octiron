OUTPUT_ARCH("riscv")
ENTRY(kernel_cxx_entry)

SECTIONS
{
	# Start of upper half address space in riscv with sv39 paging
	. = 0xFFFF800000000000
	
	.text ALIGN (4K) : {
		*(.text .text.*)
	}
	.rodata ALIGN (4K) : {
		*(.rodata .rodata.*)
	}
	.data ALIGN (4K) : {
		*(.sdata .sdata.*) *(.data .data.*)
	}
	.bss ALIGN (4K) : {
		*(.sbss .sbss.*) *(.bss .bss.*)
	}
}