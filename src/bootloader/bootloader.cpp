
#include <cstddef>

#include <colls/static_array.hpp>
#include <device_tree/device_tree.hpp>
#include <devices/uart.hpp>
#include <fmt/print.hpp>
#include <riscv/sv39.hpp>
#include <tar.hpp>
#include <types/number.hpp>

constexpr size_t BOOT_HEAP_SIZE = 32 * riscv::sv39::PAGE_SIZE;
constexpr static_array<u8, BOOT_HEAP_SIZE> BOOT_HEAP = { 0 };

extern "C"
{
    extern const char _binary____root_tar_start[];
    extern const char _binary____root_tar_end[];
}

/// For whatever reason, we're crashing if this isn't here... smt smt bss empty when zeroing?
int bss_guard_int;

void
uart_putchar(char c)
{
    uart(0x10000000).send(c);
}

extern "C" void
bootloader_cxx_entry(paddr_t dtb_blob_base)
{
    fmt::initialize(&uart_putchar);
    fmt::println("Hello, world!");
    fmt::println("Device tree blob base address: ", fmt::hex(dtb_blob_base));
    fmt::println(fmt::hex(reinterpret_cast<u64>(_binary____root_tar_start)));
    fmt::println(fmt::hex(reinterpret_cast<u64>(_binary____root_tar_end)));
    size_t hello_tar_size =
      reinterpret_cast<u64>(_binary____root_tar_end) - reinterpret_cast<u64>(_binary____root_tar_start);
    fmt::println("Size of hello.tar: ", fmt::hex(hello_tar_size));
    str_view hello_tar = str_view(_binary____root_tar_start, hello_tar_size);
    // fmt::println(hello_tar);

    ustar::archive hello_arch = ustar::archive(hello_tar);
    fmt::println(hello_arch.fetch_file("root/hello.txt").shorten(12));
    for (;;) {
    }
}
