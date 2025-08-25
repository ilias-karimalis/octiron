#pragma once

#include <types/byte_view.h>
#include <types/str_view.h>

struct ramfs_ustar
{
    /// Parses a ramfs_ustar from an immutable view of memory.
    static ramfs_ustar parse_from_view(byte_view blob);

    /// Fetchs a byte_view with the contentof a specific file given its filepath.
    byte_view fetch_file(str_view file_path);

    /// An immutable view of ustar archive in memory.
    byte_view m_ustar_blob;
    /// The number of files in the archive.
    size_t m_file_count;
    /// A list of file names in the archive
    str_view* m_file_names;
    /// A list of file content in the archive
    byte_view* m_files;
};