#include "fmt/assert.h"
#include <filesystem/ramfs_ustar.h>

struct ramfs_ustar
parse_from_view(byte_view blob)
{
    // size_t file_count = 0;
    byte_view curr_hdr = blob;
    while (curr_hdr[0] != '\0') {
        // file_count++;
        // size_t file_size =
    }
    todo("Come back to this!!!");
}