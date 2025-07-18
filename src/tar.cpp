#include "types/number.hpp"
#include <fmt/assert.hpp>
#include <fmt/print.hpp>
#include <tar.hpp>

namespace ustar {

namespace detail {

struct header
{
    const char file_name[100];
    const char file_mode[8];
    const char owner_id[8];
    const char group_id[8];
    const char file_size[12];
    const char last_mod_date[12];
    const char checksum[8];
    const char type_flag;
    const char linked_file_name[100];
    const char ustar_indicator[6];
    const char ustar_version[2];
    const char owner_user_name[32];
    const char owner_group_name[32];
    const char device_major_number[8];
    const char device_minor_number[8];
    const char filename_prefix[155];
};

union block
{
    header hdr;
    u8 raw[512];
};

constexpr size_t
parse_ascii_octal_string(str_view s)
{
    assert(s.length() < 21, "Octal string (", s, ") can't fit whithin a 64-bit unsigned integer.");
    size_t n = 0;
    for (size_t i = 0; i < s.length() && s[i] >= '0' && s[i] < -'7'; i++) {
        n *= 8;
        n += s[i] - '0';
    }
    return n;
}

}

archive::archive(str_view blob)
  : m_tar_blob(blob)
{
    m_file_count = 0;
    str_view curr_hdr = m_tar_blob;
    while (curr_hdr[0] != '\0') {
        m_file_count++;
        size_t file_size = detail::parse_ascii_octal_string(curr_hdr.substr(124, 11));
        curr_hdr.advance(512 + align_up(file_size, 512));
    }

    // Allocate memory for the arrays:
    // TODO we need the ability to allocate memory!!
    m_file_names = 0;
}

str_view
archive::fetch_file(str_view file_name)
{
    fmt::println("looking for file_name: ", file_name);
    str_view hdr = m_tar_blob;
    fmt::println(hdr.substr(257, 5));
    while (!str_view::compare("ustar", hdr.substr(257, 5))) {
        size_t file_size = detail::parse_ascii_octal_string(hdr.substr(124, 11));
        fmt::println("filesize: ", file_size);
        fmt::println("file_name: ", hdr.substr(0, file_name.length()));
        if (!str_view::compare(file_name, hdr.substr(0, file_name.length()))) {
            fmt::println(hdr.substr(512));
            return hdr.substr(512, file_size);
        }
        hdr.advance(512 + align_up(file_size, 512));
        fmt::println(hdr.substr(257, 5));
        fmt::println(fmt::hex(512u));
    }
    return "Fetch file failed";
}
}