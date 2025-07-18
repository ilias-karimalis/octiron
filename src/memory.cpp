#include <memory.hpp>

namespace mem {
void
fill(void* dest, u8 c, size_t count)
{
    u8* dst = reinterpret_cast<u8*>(dest);
    for (size_t i = 0; i < count; i++) {
        dst[i] = c;
    }
}
}
