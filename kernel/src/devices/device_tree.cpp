#include <allocators/bump.h>
#include <cinttypes>
#include <cstring>
#include <devices/device_tree.h>
#include <fmt/assert.h>
#include <panic.h>
#include <types/byte_view.h>
#include <types/error.h>
#include <types/number.h>
#include <types/stack.h>
#include <types/str_view.h>

namespace dt {

struct header
{
    /// The magic number for this forrmat, must contain the value 0xD00DFEED (big-endian).
    u32 magic;
    /// The total size in bytes of the device tree structure.
    u32 total_size;
    /// Offset in bytes to the structure block.
    u32 offset_structs;
    /// Offset in bytes to the strings block.
    u32 offset_strings;
    /// Offset in bytes to the memory reservation block.
    u32 offset_rsvmap;
    /// The version of the devicetree data structure.
    u32 version;
    u32 compatible_version;
    u32 boot_cpuid;
    u32 size_strings;
    u32 size_structs;
};

struct property
{
    enum class type
    {
        /// Property that has not been fully evaluated yet.
        RAW,
        /// List of compatible devices for driver selection.
        COMPATIBLE,
        /// Specifies the manufacturer's model number for the device.
        MODEL,
        /// Specifies a uuid for the node in the tree.
        PHANDLE,
        STATUS,
        ADDRESS_CELLS,
        SIZE_CELLS,
        REG,
        VIRTUAL_REG,
        DMA_COHERENCE,
        DEVICE_TYPE,
        RANGES,
        /// Because the hierarchy of the nodes in the interrupt tree might not match the devicetree,
        /// the interrupt-parent property is available to make the definition of an interrupt parent
        /// explicit. The value is the phandle to the interrupt parent. If this property is missing
        /// from a device, its interrupt parent is assumed to be its devicetree parent.
        INTERRUPT_PARENT,
        /// The presence of an interrupt-controller property defines a node as an interrupt
        /// controller node.
        INTERRUPT_CONTROLLER,
        /// The #interrupt-cells property defines the number of cells required to encode an
        /// interrupt specifier for an interrupt domain.
        INTERRUPT_CELLS,
        ///
        INTERRUPT_MAP,
    };

    enum class device_status
    {
        OKAY,
        DISABLED,
        RESERVED,
        FAIL,
        FAIL_WITH_REASON,
    };

    /// Name of the property.
    str_view name;
    /// The next property in the list.
    struct property* next_property;
    /// The type of this property.
    enum type type;
    /// The property specific type specific data
    union
    {
        /// Raw property data, used for properties that have not been fully evaluated yet.
        byte_view raw;
        /// Null-terminated list specifying device compatibility for driver selection.
        str_view* compatible_array;
        str_view model;
        u32 phandle;
        struct
        {
            enum device_status value;
            str_view reason;
        } status;
        u32 address_cells;
        u32 size_cells;
        bool dma_coherence;
        str_view device_type;
        struct
        {
            size_t n_pairs;
            void* address_array;
            void* size_array;
        } reg;
        struct
        {
            size_t n_trips;
            void* cbus_address_array;
            void* pbus_address_array;
            void* size_array;
        } range;
        u32 virtual_reg;
        u32 interrupt_parent;
        u32 interrupt_cells;
        struct
        {
            size_t n_entries;
            void* child_unit_addresses;
            void* child_interrupt_specifiers;
            void* interrupt_parents;
            void* parent_unit_addresses;
            void* parent_interrupt_specifiers;
        };
    } data;
};

struct node
{
    /// Name of the node.
    str_view name;
    /// The number of the <u32> cells used to encode the address field in this node's reg property.
    u32 address_cells;
    /// The number of the <u32> cells used to encode the size field in this node's reg property.
    u32 size_cells;
    /// Linked list of properties of the node
    struct property* properties;
    /// Parent node
    struct node* parent;
    /// Linked list of child nodes
    struct node* children;
    /// Next sibling node
    struct node* next_sibling;
};

struct reserved_region
{
    /// The physical address of the reserved region.
    paddr_t address;
    /// The size of the reserved region in bytes
    size_t size;
};

/// List of reserved memory regions.
stack<reserved_region> reserved_regions = {};
/// List of device tree nodes. nodes[0] is the root node.
stack<struct node> nodes = {};
/// List of device tree properties.
stack<struct property> properties = {};
/// Bump allocator for
bump_alloc bump = {};
struct node* root = nullptr;
bool initialized = false;

size_t
parse_node(struct node** current, const u8* structures, size_t offset)
{
    str_view name = str_view::from_null_term((const char*)structures + offset);
    struct node* new_node = nodes.emplace_back(name, 0u, 0u, nullptr, *current, nullptr, nullptr);
    if (*current == nullptr) [[unlikely]] {
        *current = new_node;
        return offset + align_up(name.length() + 1, sizeof(u32));
    }

    new_node->next_sibling = (*current)->children;
    (*current)->children = new_node;
    *current = new_node;
    return offset + align_up(name.length() + 1, sizeof(u32));
}

size_t
pre_parse_property(struct node* current, const u8* structures, const u8* strings, size_t offset)
{
    u32 property_length = num::read_big_endian<u32>(structures + offset);
    offset += sizeof(u32);
    u32 name_offset = num::read_big_endian<u32>(structures + offset);
    offset += sizeof(u32);

    str_view property_name = str_view::from_null_term((const char*)strings + name_offset);
    byte_view property_value = byte_view(structures + offset, property_length);
    offset += align_up(property_length, sizeof(u32));

    struct property* new_property = properties.emplace_back(
      property_name, current->properties, property::type::RAW, property_value);
    current->properties = new_property;
    return offset;
}

void
property_rewrite_ranges(struct node* node, struct property* prop)
{
    const u8* value_buffer = prop->data.raw.data();
    size_t value_len = prop->data.raw.length();
    if (value_len == 0) {
        prop->type = property::type::RANGES;
        prop->data.range = {
            .n_trips = 0,
            .cbus_address_array = nullptr,
            .pbus_address_array = nullptr,
            .size_array = nullptr,
        };
        return;
    }

    u32 child_address_size = sizeof(u32) * node->address_cells;
    u32 parent_address_size = sizeof(u32) * node->parent->address_cells;
    u32 size_size = sizeof(u32) * node->size_cells;
    size_t n_trips = value_len / (child_address_size + parent_address_size + size_size);

    assert(n_trips > 0);
    assert(value_len % (child_address_size + parent_address_size + size_size) == 0);
    assert(node->address_cells <= 3);
    assert(node->size_cells <= 2);

    void* cbus_address_array = bump.alloc_aligned(child_address_size * n_trips, child_address_size);
    void* pbus_address_array = bump.alloc_aligned(child_address_size * n_trips, child_address_size);
    void* size_array = bump.alloc_aligned(size_size * n_trips, size_size);

    for (size_t i = 0, j = 0; i < n_trips; i++) {
        switch (node->address_cells) {
            case 1:
                ((u32*)cbus_address_array)[i] = num::read_big_endian<u32>(value_buffer + j);
                break;
            case 2:
                ((u64*)cbus_address_array)[i] = num::read_big_endian<u64>(value_buffer + j);
                break;
            case 3:
                ((u64*)cbus_address_array)[i] = num::read_big_endian<u128>(value_buffer + j);
                break;
            default:
                __builtin_unreachable();
        }
        j += child_address_size;

        switch (node->parent->address_cells) {
            case 1:
                ((u32*)pbus_address_array)[i] = num::read_big_endian<u32>(value_buffer + j);
                break;
            case 2:
                ((u64*)pbus_address_array)[i] = num::read_big_endian<u64>(value_buffer + j);
                break;
            case 3:
                ((u64*)pbus_address_array)[i] = num::read_big_endian<u128>(value_buffer + j);
                break;
            default:
                __builtin_unreachable();
        }
        j += parent_address_size;

        switch (node->size_cells) {
            case 0:
                break;
            case 1:
                ((u32*)size_array)[i] = num::read_big_endian<u32>(value_buffer + j);
                break;
            case 2:
                ((u64*)size_array)[i] = num::read_big_endian<u64>(value_buffer + j);
                break;
            default:
                __builtin_unreachable();
        }
        j += size_size;
    }

    prop->type = property::type::RANGES;
    prop->data.range.n_trips = n_trips;
    prop->data.range.cbus_address_array = cbus_address_array;
    prop->data.range.pbus_address_array = pbus_address_array;
    prop->data.range.size_array = size_array;
}

void
property_rewrite_reg(struct node* node, struct property* prop)
{
    byte_view bv = prop->data.raw;
    u32 address_size = sizeof(u32) * node->parent->address_cells;
    u32 size_size = sizeof(u32) * node->parent->size_cells;
    size_t n_pairs = bv.length() / (address_size + size_size);

    assert(n_pairs > 0);
    assert(bv.length() % (address_size + size_size) == 0,
           bv.length(),
           " ",
           address_size,
           " ",
           size_size);
    assert(node->parent->address_cells <= 3);
    assert(node->parent->size_cells <= 2);

    void* address_array = bump.alloc_aligned(address_size * n_pairs, address_size);
    void* size_array = bump.alloc_aligned(size_size * n_pairs, size_size);
    assert((address_size == 0) ? address_array == NULL : address_array != NULL);
    assert((size_size == 0) ? size_array == NULL : size_array != NULL);

    for (size_t i = 0, j = 0; i < n_pairs; i++) {
        switch (node->parent->address_cells) {
            case 0:
                break;
            case 1:
                ((u32*)address_array)[i] = num::read_big_endian<u32>(bv.data() + j);
                break;
            case 2:
                ((u64*)address_array)[i] = num::read_big_endian<u64>(bv.data() + j);
                break;
            case 3:
                ((u128*)address_array)[i] = num::read_big_endian<u128>(bv.data() + j);
                break;
            default:
                __builtin_unreachable();
        }
        j += address_size;

        switch (node->parent->size_cells) {
            case 0:
                break;
            case 1:
                ((u32*)size_array)[i] = num::read_big_endian<u32>(bv.data() + j);
                break;
            case 2:
                ((u64*)size_array)[i] = num::read_big_endian<u64>(bv.data() + j);
                break;
            default:
                __builtin_unreachable();
        }
        j += size_size;
    }

    prop->type = property::type::REG;
    prop->data.reg.n_pairs = n_pairs;
    prop->data.reg.address_array = address_array;
    prop->data.reg.size_array = size_array;
}

error
recursive_property_rewrite(struct node* node)
{
    error err = error();

    for (struct property* prop = node->properties; prop != nullptr; prop = prop->next_property) {
        if (str_view::compare("compatible", prop->name) == 0) {
            byte_view raw_view = prop->data.raw;
            size_t num_strings = 0;
            for (size_t i = 0; i < raw_view.length(); i++) {
                if (raw_view[i] == '\0') {
                    num_strings++;
                }
            }

            prop->data.compatible_array = (str_view*)bump.alloc_aligned(
              sizeof(str_view*) * (num_strings + 1), sizeof(str_view*));
            assert(prop->data.compatible_array != nullptr);
            for (size_t i = 0, j = 0; i < raw_view.length(); j++) {
                prop->data.compatible_array[j] =
                  str_view::from_null_term((const char*)&raw_view[i]);
                i += prop->data.compatible_array[j].length() + 1;
            }
            prop->data.compatible_array[num_strings] = str_view();
            prop->type = property::type::COMPATIBLE;
        } else if (str_view::compare("model", prop->name) == 0) {
            str_view model_string = str_view::from_byte_view(prop->data.raw);
            prop->type = property::type::MODEL;
            prop->data.model = model_string;
        } else if (str_view::compare("phandle", prop->name) == 0) {
            prop->type = property::type::PHANDLE;
            u32 phandle = num::read_big_endian<u32>(prop->data.raw.data());
            prop->data.phandle = phandle;
        } else if (str_view::compare("status", prop->name) == 0) {
            prop->type = property::type::STATUS;
            str_view status_string = str_view::from_byte_view(prop->data.raw);
            if (str_view::compare("okay", status_string) == 0) {
                prop->data.status.value = property::device_status::OKAY;
                prop->data.status.reason = str_view();
            } else if (str_view::compare("disabled", status_string) == 0) {
                prop->data.status.value = property::device_status::DISABLED;
                prop->data.status.reason = str_view();
            } else if (str_view::compare("reserved", status_string) == 0) {
                prop->data.status.value = property::device_status::RESERVED;
                prop->data.status.reason = str_view();
            } else if (str_view::compare("fail", status_string) == 0) {
                prop->data.status.value = property::device_status::FAIL;
                prop->data.status.reason = str_view();
            } else if (str_view::compare("fail-", status_string.advance(5)) == 0) {
                prop->data.status.value = property::device_status::FAIL_WITH_REASON;
                prop->data.status.reason = status_string.advance(5);
            }
        } else if (str_view::compare("#address-cells", prop->name) == 0) {
            prop->type = property::type::ADDRESS_CELLS;
            prop->data.address_cells = num::read_big_endian<u32>(prop->data.raw.data());
            if (prop->data.address_cells > 3) {
                return ErrorCode::DT_ADDRESS_CELLS_TOO_LARGE;
            }
            node->address_cells = prop->data.address_cells;
        } else if (str_view::compare("#size-cells", prop->name) == 0) {
            prop->type = property::type::SIZE_CELLS;
            prop->data.size_cells = num::read_big_endian<u32>(prop->data.raw.data());
            if (prop->data.size_cells > 2) {
                return ErrorCode::DT_SIZE_CELLS_TOO_LARGE;
            }
            node->size_cells = prop->data.size_cells;
        } else if (str_view::compare("dma-coherent", prop->name) == 0) {
            prop->type = property::type::DMA_COHERENCE;
            prop->data.dma_coherence = true;
        } else if (str_view::compare("dma-noncoherent", prop->name) == 0) {
            prop->type = property::type::DMA_COHERENCE;
            prop->data.dma_coherence = false;
        } else if (str_view::compare("device_type", prop->name) == 0) {
            prop->type = property::type::DEVICE_TYPE;
            prop->data.device_type = str_view::from_byte_view(prop->data.raw);
        } else if (str_view::compare("virtual-reg", prop->name) == 0) {
            prop->type = property::type::VIRTUAL_REG;
            prop->data.virtual_reg = num::read_big_endian<u32>(prop->data.raw.data());
        } else if (str_view::compare("interrupt-parent", prop->name) == 0) {
            prop->type = property::type::INTERRUPT_PARENT;
            prop->data.interrupt_parent = num::read_big_endian<u32>(prop->data.raw.data());
        } else if (str_view::compare("#interrupt-cells", prop->name) == 0) {
            prop->type = property::type::INTERRUPT_CELLS;
            prop->data.interrupt_cells = num::read_big_endian<u32>(prop->data.raw.data());
        } else if (str_view::compare("interrupts", prop->name) == 0) {
            //            todo("Implement parsing of 'interrupts' property");
        } else if (str_view::compare("interrupt-map", prop->name) == 0) {
            //            todo("Implement parsing of 'interrupt-map' property");
        } else if (str_view::compare("interrupt-map-mask", prop->name) == 0) {
            //            todo("Implement parsing of 'interrupt-map-mask' property");
        } else if (str_view::compare("interrupt-controller", prop->name) == 0) {
            prop->type = property::type::INTERRUPT_CONTROLLER;
        } else if (str_view::compare("regmap", prop->name) == 0) {
            //            todo("Implement parsing of 'regmap' property");
        } else if (str_view::compare("value", prop->name) == 0) {
            //            todo("Implement parsing of 'value' property");
        }

        else if (str_view::compare("reg", prop->name) == 0 ||
                 str_view::compare("ranges", prop->name) == 0 ||
                 str_view::compare("bus-ranges", prop->name) == 0) {
            // property_rewrite_ranges(node, prop);
        } else {
            fmt::println("Unhandled device tree property: ", prop->name);
        }
    }

    // Second lap through the properties to work on `reg`, `ranges`, and `bus-ranges`
    for (struct property* prop = node->properties; prop != nullptr; prop = prop->next_property) {
        if (str_view::compare("reg", prop->name) == 0) {
            property_rewrite_reg(node, prop);
        } else if (str_view::compare("ranges", prop->name) == 0 ||
                   str_view::compare("bus-ranges", prop->name) == 0) {
            property_rewrite_ranges(node, prop);
        }
    }

    for (struct node* child = node->children; child != nullptr; child = child->next_sibling) {
        if ((err = recursive_property_rewrite(child)).is_err()) {
            return err;
        }
    }
    return err;
}

static const u32 STRUCTURE_BEGIN_NODE = 0x01;
static const u32 STRUCTURE_END_NODE = 0x02;
static const u32 STRUCTURE_PROP = 0x03;
static const u32 STRUCTURE_NOP = 0x04;
static const u32 STRUCTURE_END = 0x09;

error
parse_from_blob(const u8* dtb)
{

    const struct header* hdr = (const struct header*)dtb;
    if (0xD00DFEED != num::flip_endianness(hdr->magic)) {
        return ErrorCode::DT_MAGIC_NUMBER;
    }

    // size_t size = num::flip_endianness(hdr->total_size);

    u64* rsvmap = (u64*)(dtb + num::flip_endianness(hdr->offset_rsvmap));
    while (rsvmap[0] != 0 && rsvmap[1] != 0) {
        reserved_regions.emplace_back(num::flip_endianness(rsvmap[0]),
                                      num::flip_endianness(rsvmap[1]));
    }

    const u8* structures = dtb + num::flip_endianness(hdr->offset_structs);
    const u8* strings = dtb + num::flip_endianness(hdr->offset_strings);
    // byte_view structures = byte_view(dtb + num::flip_endianness(hdr->offset_structs),
    //                                  num::flip_endianness(hdr->size_structs));
    // byte_view

    struct node* pseudo_root_node = nullptr;
    size_t offset = 0;
    size_t depth = 0;

    struct node* current = pseudo_root_node;
    for (;;) {
        assert(offset % 4 == 0, "Accesses muust be 4 bytes aligned.");
        u32 token = num::read_big_endian<u32>(structures + offset);
        offset += sizeof(u32);

        switch (token) {
            case STRUCTURE_BEGIN_NODE:
                offset = parse_node(&current, structures, offset);
                depth++;
                break;

            case STRUCTURE_END_NODE:
                current = current->parent;
                depth--;
                break;

            case STRUCTURE_PROP:
                offset = pre_parse_property(current, structures, strings, offset);
                break;

            case STRUCTURE_NOP:
                break;

            case STRUCTURE_END:
                if (current != nullptr) {
                    panic("While parsing the device tree, found a STRUCTURES_END token while the "
                          "current node is not the root node. Depth: ",
                          depth);
                }
                goto dtb_property_rewrite_pass;

            default:
                panic("While parsing the device tree, found an unknown structure token type: ",
                      fmt::hex(token));
        }
    }

dtb_property_rewrite_pass:
    if (nodes.m_size == 0) {
        return error(ErrorCode::DT_NO_NODES);
    }
    root = &nodes[0];

    /// We're now ready to properly rewrite the device tree properties.
    root->address_cells = 2;
    root->size_cells = 1;
    error err = recursive_property_rewrite(root);
    if (err.is_err()) {
        return err.push(ErrorCode::DT_REWRITE_FAILED);
    }

    initialized = true;
    root->name = str_view("/");
    return error(ErrorCode::SUCCESS);
}

void
print_node_recursive(struct node* node, int depth)
{
    if (node == nullptr) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        fmt::print("  ");
    }
    fmt::println("Node: ", node->name, " {");
    for (struct property* prop = node->properties; prop != nullptr; prop = prop->next_property) {
        for (int i = 0; i < depth + 1; i++) {
            fmt::print("  ");
        }
        fmt::print("Property: ", prop->name, " = ");
        switch (prop->type) {
            case property::type::RAW:
                fmt::println("RAW (length: ", prop->data.raw.length(), ")");
                break;
            case property::type::COMPATIBLE:
                fmt::print("[");
                for (size_t i = 0; prop->data.compatible_array[i].length() != 0; i++) {
                    if (i != 0) {
                        fmt::print(", ");
                    }
                    fmt::print(prop->data.compatible_array[i]);
                }
                fmt::println("]");
                break;
            case property::type::MODEL:
                fmt::println(prop->data.model);
                break;
            case property::type::PHANDLE:
                fmt::println(fmt::hex(prop->data.phandle));
                break;
            case property::type::STATUS:
                switch (prop->data.status.value) {
                    case property::device_status::OKAY:
                        fmt::println("OKAY");
                        break;
                    case property::device_status::DISABLED:
                        fmt::println("DISABLED");
                        break;
                    case property::device_status::RESERVED:
                        fmt::println("RESERVED");
                        break;
                    case property::device_status::FAIL:
                        fmt::println("FAIL");
                        break;
                    case property::device_status::FAIL_WITH_REASON:
                        fmt::println("FAIL: ", prop->data.status.reason);
                        break;
                }
                break;
            case property::type::ADDRESS_CELLS:
                fmt::println(prop->data.address_cells);
                break;
            case property::type::SIZE_CELLS:
                fmt::println(prop->data.size_cells);
                break;
            case property::type::DMA_COHERENCE:
                fmt::println(prop->data.dma_coherence ? "true" : "false");
                break;
            case property::type::DEVICE_TYPE:
                fmt::println(prop->data.device_type);
                break;
            case property::type::REG:
                fmt::println("REG (", prop->data.reg.n_pairs, " pairs)");
                break;
            case property::type::RANGES:
                fmt::println("RANGES (", prop->data.range.n_trips, " trips)");
                break;
            case property::type::VIRTUAL_REG:
                fmt::println(fmt::hex(prop->data.virtual_reg));
                break;
            case property::type::INTERRUPT_PARENT:
                fmt::println(fmt::hex(prop->data.interrupt_parent));
                break;
            case property::type::INTERRUPT_CONTROLLER:
                fmt::println("true");
                break;
            case property::type::INTERRUPT_CELLS:
                fmt::println(prop->data.interrupt_cells);
                break;
            case property::type::INTERRUPT_MAP:
                fmt::println("INTERRUPT_MAP (", prop->data.n_entries, " entries)");
                break;
        }
    }

    for (struct node* child = node->children; child != nullptr; child = child->next_sibling) {
        print_node_recursive(child, depth + 1);
    }
}

void
print_device_tree()
{

    if (!initialized) {
        fmt::println("Device tree not initialized.");
        return;
    }

    fmt::println("Printing the device tree:");
    print_node_recursive(root, 0);
}
}