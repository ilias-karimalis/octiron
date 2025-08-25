#include <uart.h>

uart::uart(void* base_address)
	: m_base(reinterpret_cast<u8*>(base_address))
{}

void uart::send(char c)
{
	*m_base = c;
}

char uart::receive()
{
	while ((m_base[0b101] & 0x01) == 0);
	return static_cast<char>(*m_base);
}