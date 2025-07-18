#include <freestanding.hpp>

#include <riscv/sv39.hpp>

#include <kernel/devices/uart.hpp>
#include <kernel/page_alloc.hpp>
#include <fmt/print.hpp>
#include <fmt/assert.hpp>
#include <colls/slab_alloc.hpp>

constexpr size_t EARLY_HEAP_SIZE = 128 * riscv::sv39::PAGE_SIZE;

void uart_putchar(char c) {
	Uart(0x10000000).send(c);
}


extern "C" void kernel_cxx_entry() {
	fmt::initialize(&uart_putchar);
	fmt::println("In the kernel!");
	for (;;);
}
