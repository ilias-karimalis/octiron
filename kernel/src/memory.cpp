#include <memory.h>

namespace mem {
void
fill(void* dest, u8 c, size_t count)
{
    u8* dst = reinterpret_cast<u8*>(dest);
    for (size_t i = 0; i < count; i++) {
        dst[i] = c;
    }
}

ssize_t
cmp(const void* lhs, const void* rhs, size_t count)
{
    const u8* lhs_u8 = static_cast<const u8*>(lhs);
    const u8* rhs_u8 = static_cast<const u8*>(rhs);

    if (lhs_u8 == rhs_u8 || count == 0) {
        return 0;
    }

    while (--count && *lhs_u8 == *rhs_u8) {
        lhs_u8++;
        rhs_u8++;
    }
    return *lhs_u8 - *rhs_u8;
}

void
copy(const void* src, void* dst, size_t count)
{
    const u8* src_u8 = static_cast<const u8*>(src);
    u8* dst_u8 = static_cast<u8*>(dst);

    for (size_t i = 0; i < count; i++) {
        dst_u8[i] = src_u8[i];
    }
}
} // namespace mem
