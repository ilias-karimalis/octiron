#pragma once

#include <cstddef>
#include <types/number.h>

namespace mem {
void
fill(void* dest, u8 c, size_t count);

ssize_t
cmp(const void* lhs, const void* rhs, size_t count);

void
copy(const void* src, void* dst, size_t count);

}
