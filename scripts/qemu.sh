#!/usr/bin/env bash

# This script is used to run QEMU with the specified parameters.
# Usage: ./scripts/qemu.sh using_gdb bootloader_elf kernel_elf

set -xeuo pipefail

# If using_gdb is true, start QEMU with GDB support.
if [ "$1" -eq 0 ]; then
    echo "Attach to this qemu process by running: gdb-multiarch $2 -ex \"set architecture:rv64\" -ex \"target remote localhost:1234\""
    qemu-system-riscv64   \
        -M virt           \
        -cpu rv64         \
        -smp 2            \
        -m 4G             \
        -nographic        \
        -serial mon:stdio \
        -bios default     \
        -kernel "$2"     \
        -s                \
        -S
    exit 0
fi

qemu-system-riscv64   \
    -M virt           \
    -cpu rv64         \
    -smp 2            \
    -m 4G             \
    -nographic        \
    -serial mon:stdio \
    -kernel "$2"
        # -bios default     \
