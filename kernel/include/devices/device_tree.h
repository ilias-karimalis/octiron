#pragma once

#include <types/byte_view.h>
#include <types/dynamic_array.h>

namespace dt {

/// Initializes the device tree structure by parsing the device tree blob. This procedure allocates
/// its own memory for its internal structures.
error
parse_from_blob(const u8* dtb);

// struct device_tree
// {

//     static result<device_tree> parse_from_blob
// };

// struct device_tree
// {
//     static device_tree parse_from_view(byte_view blob);

//     struct reserved_region
//     {
//         paddr_t address;
//         size_t size;
//     };

//     dynamic_array<reserved_region> m_reserved_regions;
// };

}