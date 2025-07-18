# BootLoader

.option norvc

.section .text.init
.global bootloader_asm_entry
bootloader_asm_entry:

.option push
.option norelax
	la 		gp, __boot_global_pointer
.option pop	

# Set all bytes in the BSS section to zero
# 	la 		a0, __boot_bss_start
# 	la  	a2, __boot_bss_end
# 	bgeu    a0, a2, 2f
# 1:
# 	sd 		zero, (a0)
# 	addi 	a0, a0, 8
# 	bltu	a0, a2, 1b
# 2:
    la      a0, __boot_bss_start       # Load address of BSS start
    la      a2, __boot_bss_end         # Load address of BSS end
    beq     a0, a2, 2f                 # If BSS start == BSS end, skip loop
1:
    sd      zero, (a0)                 # Store zero at the address in a0
    addi    a0, a0, 8                  # Increment address by 8 bytes
    bltu    a0, a2, 1b                 # Continue loop until a0 >= a2

2:
	# Initialize the kernel stack
	la		sp, __boot_stack_end
	# a1 contains the address of the physical address of the device tree blob, we pass it to bootloader_cxx_entry
	# as it's first argument
	mv      a0, a1
	# Jump to cpp entry point
	j bootloader_cxx_entry
	# If we ever get back here just spin waiting on an interruppt
3: 
	wfi
	j 3b