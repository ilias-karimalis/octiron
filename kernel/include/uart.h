#pragma once

#include <types/number.h>

struct uart
{
    /// Initializes a uart object with the given base address.
    explicit uart(void* base_address);
    /// Sends a character over the uart.
    void send(char c);
    /// Receives a character over the uart.
    char receive();

    /// The uart base address.
    volatile u8* m_base;
};