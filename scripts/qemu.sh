#!/bin/bash

IMAGE_FILE=$1
BIOS_FILE=$2

# Detect OS and set appropriate display option
if [[ "$OSTYPE" == "darwin"* ]]; then
    DISPLAY_OPTION="cocoa"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    DISPLAY_OPTION="gtk"
else
    echo "Error: Unsupported operating system: $OSTYPE"
    echo "This script only supports macOS (darwin) and Linux (linux-gnu)"
    exit 1
fi

qemu-system-riscv64 \
    -machine virt \
    -m 2G \
    -cpu rv64 \
    -device qemu-xhci \
    -device usb-kbd \
    -device usb-mouse \
    -device ramfb \
    -drive if=pflash,unit=0,format=raw,file=${BIOS_FILE},readonly=on \
    -cdrom ${IMAGE_FILE} \
    -display ${DISPLAY_OPTION} \
    -serial mon:stdio 

