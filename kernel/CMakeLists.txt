# Enable assertions in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(ENABLE_ASSERTIONS)
    message(STATUS "Assertions enabled (Debug build)")
endif()

add_executable(Kernel.elf
	src/kernel_entry.cpp
    src/uart.cpp
    src/pmm.cpp
    src/memory.cpp
    src/fmt/fmt.cpp
    src/limine/platform_info.cpp
    src/devices/device_tree.cpp
    src/allocators/bump.cpp
)
target_include_directories(Kernel.elf PRIVATE include/)
target_compile_options(Kernel.elf PRIVATE -Wall -Werror -mcmodel=medany -ffreestanding -nostdlib -fno-exceptions -fno-stack-protector -fno-rtti -fno-use-cxa-atexit)
set(KERNEL_LD_SCRIPT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/kernel_limine.ld")
target_link_options(Kernel.elf PRIVATE  -T${KERNEL_LD_SCRIPT_PATH} -nostdlib -ffreestanding -Wl,-Map=Kernel.elf.map)
