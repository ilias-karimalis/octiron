#include <devices/device_tree.h>
#include <fmt/print.h>
#include <limine/platform_info.h>
#include <panic.h>
#include <pmm.h>
#include <types/number.h>
#include <uart.h>

void
uart_putchar(char c)
{
    uart(limine::hhdm_phys_to_virt(0x10000000)).send(c);
}

extern "C" void
kernel_cxx_entry()
{
    fmt::initialize(&uart_putchar);

    const struct limine::platform_info* pinfo = limine::parse_platform_info();
    assert(LIMINE_PAGING_MODE_RISCV_SV39 == pinfo->mode,
           "For now we only support SV39 style paging.");

    pmm::initialize(pmm::Policy::FIRST_FIT);
    for (size_t i = 0; i < pinfo->memmap_count; i++) {
        if (pinfo->memmap[i].type == LIMINE_MEMMAP_USABLE) {
            error err = pmm::add_region(pinfo->memmap[i].base, pinfo->memmap[i].length);
            if (err.is_err()) {
                panic(err.str());
            }
        }
    }
    fmt::println("PMM free bytes: ", fmt::hex(pmm::free_memory()));

    error err = dt::parse_from_blob((const u8*)pinfo->device_tree_blob);
    assert(err.is_ok(), err.str());

    limine_framebuffer* framebuffer = pinfo->framebuffers[0];
    for (std::size_t i = 0; i < 100; i++) {
        volatile std::uint32_t* fb_ptr = static_cast<volatile std::uint32_t*>(framebuffer->address);
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }

    dt::print_device_tree();

    for (;;)
        ;
}