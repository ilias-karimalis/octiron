#include <devices/uart.hpp>

void uart::send(char c) {
	*m_base = c;
}

char uart::receive() {
	while ((m_base[0b101] & 0x01) == 0);
	return static_cast<char>(*m_base);
}
