cmake_minimum_required(VERSION 3.16)
project(octiron CXX ASM)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(ENABLE_ASSERTIONS)
    message(STATUS "Assertions enabled (Debug build)")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CXX_OPTIONS )
#set(ASM_OPTIONS )

# Make sure to generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the bootloader executable
set(BOOT bootloader.elf)
file(GLOB_RECURSE BOOT_SRC_ASM src/bootloader/*.S)
file(GLOB_RECURSE BOOT_SRC_CXX
        src/bootloader/*.cpp
        src/riscv/*.cpp
        src/devices/*.cpp
)
set(BOOT_SRC ${BOOT_SRC_ASM} ${BOOT_SRC_CXX})
set(BOOT_LINK_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/bootloader_qemu_virt.ld")
add_executable(${BOOT} ${BOOT_SRC})
target_include_directories(${BOOT} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(${BOOT} PRIVATE -Wall -Wextra -Werror -mcmodel=medany -ffreestanding -nostdlib -fno-exceptions -fno-stack-protector -fno-rtti)
set_source_files_properties(${BOOT_SRC_ASM} PROPERTIES COMPILE_FLAGS "-nostdlib -ffreestanding -x assembler-with-cpp")
target_link_options(${BOOT} PRIVATE -T${BOOT_LINK_SCRIPT} -nostdlib -ffreestanding -Wl,-Map=${BOOT}.map)

# Custom command to run qemu
add_custom_target(
        run
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/qemu.sh 1 ${BOOT}
        DEPENDS ${BOOT}
        COMMENT "Running QEMU with bootloader ${BOOT}"
)

# Custom command to run qemu for debugging
add_custom_target(
        debug
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/qemu.sh 0 ${BOOT}
        DEPENDS ${BOOT}
        COMMENT "Running QEMU in debug mode with bootloader ${BOOT}"
)

