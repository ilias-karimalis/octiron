#pragma once

#include <types/byte_view.h>
#include <types/dynamic_array.h>

namespace dt {

/// Initializes the device tree structure by parsing the device tree blob. This procedure allocates
/// its own memory for its internal structures.
error
parse_from_blob(const u8* dtb);

void
print_device_tree();

}