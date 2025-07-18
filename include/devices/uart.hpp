#pragma once

#include <fmt/printer.hpp>
#include <types/number.hpp>

class uart
{
public:
    /// Initializes a Uart class object with the given base address.
    constexpr explicit uart(paddr_t base)
      : m_base(reinterpret_cast<u8*>(base))
    {
    }

    /// Sends a character over the Uart.
    void send(char c);

    /// Receives a character over the Uart.
    char receive();

private:
    /// Implements printing of the Uart class.
    friend fmt::printer<uart>;

    /// UART base address
    volatile u8* m_base;
};

/// Printer specialization for Uart
template<>
struct fmt::printer<uart>
{
    static void print(const putCharFunc put_char, const uart& uart)
    {
        printer<const char*>::print(put_char, "Uart Device @ ");
        printer<hex<paddr_t>>::print(put_char, hex(reinterpret_cast<paddr_t>(uart.m_base)));
    }
};
