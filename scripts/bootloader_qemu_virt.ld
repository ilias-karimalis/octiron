OUTPUT_ARCH("riscv")

ENTRY(bootloader_asm_entry)

MEMORY {
    ram (wxa!ri): ORIGIN = 0x80200000, LENGTH = 1M
}

PHDRS
{
	text PT_LOAD;
	data PT_LOAD;
	bss  PT_LOAD;
}

SECTIONS
{
    .text : {
        PROVIDE(__boot_text_start = .);
        *(.text.init) *(.text .text.*)
        PROVIDE(__boot_text_end = .);
    } >ram AT>ram :text

    PROVIDE(__boot_global_pointer = .);

    .rodata : {
        PROVIDE(__boot_rodata_start = .);
        *(.rodata .rodata.*)
        PROVIDE(__boot_rodata_end = .);
    } >ram AT>ram :data

    .data : {
		PROVIDE(__boot_data_start = .);
		*(.sdata .sdata.*) *(.data .data.*)
		PROVIDE(__boot_data_end = .);
	} >ram AT>ram :data

	.bss : {
		PROVIDE(__boot_bss_start = .);
		*(.sbss .sbss.*) *(.bss .bss.*)
		PROVIDE(__boot_bss_end = .);
	} >ram AT>ram :bss

    . = ALIGN(4096);
	PROVIDE(__boot_stack_start = .);
	. = . + 0x8000;
	PROVIDE(__boot_stack_end = .);
}
