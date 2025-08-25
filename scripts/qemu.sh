#!/bin/bash

IMAGE_FILE=$1
BIOS_FILE=$2

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
    -display gtk \
    -serial mon:stdio 

