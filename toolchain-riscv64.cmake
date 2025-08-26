# Cross-compiling for bare-metal RISC-V (no OS)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Toolchain prefix — assumes riscv64-unknown-elf-* is in your PATH
set(CMAKE_C_COMPILER riscv64-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv64-unknown-elf-g++)
set(CMAKE_ASM_COMPILER riscv64-unknown-elf-gcc)

# Optional: Explicitly specify other tools
set(CMAKE_AR riscv64-unknown-elf-ar)
set(CMAKE_OBJCOPY riscv64-unknown-elf-objcopy)
set(CMAKE_OBJDUMP riscv64-unknown-elf-objdump)
set(CMAKE_SIZE riscv64-unknown-elf-size)
set(CMAKE_STRIP riscv64-unknown-elf-strip)

# Set ABI and architecture
set(CMAKE_C_FLAGS "-march=rv64imac -mabi=lp64" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "-march=rv64imac -mabi=lp64" CACHE STRING "" FORCE)

# Don't try to run built executables
set(CMAKE_EXECUTABLE_SUFFIX "")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# No standard libraries (unless you're linking a newlib or something similar)
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -Wl,--gc-sections" CACHE STRING "" FORCE)

