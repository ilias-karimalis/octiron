#include <colls/bump_alloc.hpp>

bump_alloc::bump_alloc(u8 *buffer, size_t buffer_size)
	: m_base(buffer), m_size(buffer_size), m_index(0) {
}

void *bump_alloc::alloc(size_t size) {
	if (size == 0 || m_index + size > m_size) return nullptr;

	void *ptr = reinterpret_cast<void *>(m_base + m_index);
	m_index += size;
	return ptr;
}

void *bump_alloc::alloc_aligned(size_t size, size_t alignment) {
	if (size == 0 || alignment == 0 || ROUND_UP(m_index, alignment) + size > m_size)
		return nullptr;

	m_index = ROUND_UP(m_index, alignment);

	void *ptr = reinterpret_cast<void *>(m_base + m_index);
	m_index += size;
	return ptr;
}

void *bump_alloc::copy(const void *src, size_t size) {
	if (src == nullptr || size == 0 || m_index + size > m_size)
		return nullptr;

	void *dest = reinterpret_cast<void *>(m_base + m_index);
	m_index += size;
	if (dest == nullptr) return nullptr;

	memcpy(dest, src, size);
	return dest;
}

const char *bump_alloc::str_copy(const char *src) {
	if (src == nullptr) return nullptr;

	size_t len = strlen(src) + 1;
	if (m_index + len > m_size) return nullptr;

	char *dst = reinterpret_cast<char *>(m_base + m_index);
	m_index += len;
	memcpy(dst, src, len);
	return dst;
}
