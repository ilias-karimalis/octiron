cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(Octiron VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)

# Set the compiler, the path here will be different on other machines:
set(CMAKE_CXX_COMPILER "/opt/riscv/bin/riscv64-unknown-elf-c++")

# Make sure to generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Directories for scripts
set(SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/scripts)

# Add the Kernel subproject
add_subdirectory(kernel)

set(IMAGE_NAME "Kernel.iso")
add_custom_target(iso
    COMMAND ${SCRIPTS_DIR}/build_iso.sh ${IMAGE_NAME} $<TARGET_FILE:Kernel.elf> ${SCRIPTS_DIR}/limine.conf ${SCRIPTS_DIR}/qemu_virt.dtb
    DEPENDS Kernel.elf
    COMMENT "Building a bootable .iso file"
)

# Custom target to trigger OVMF download
add_custom_target(download_ovmf
    COMMAND ${SCRIPTS_DIR}/download_ovmf.sh
    COMMENT "Downloading OVMF for riscv64"
)

# Custom target to run the kernel in QEMU
add_custom_target(run
    COMMAND ${SCRIPTS_DIR}/qemu.sh ${IMAGE_NAME} ovmf/ovmf-code-riscv64.fd
    DEPENDS iso download_ovmf
    COMMENT "Running Octiron in QEMU"
)