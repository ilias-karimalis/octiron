#include <types/str_view.hpp>

namespace ustar {

class archive
{
public:
    /// Creates an archive from a str_view of the archive blob.
    explicit archive(str_view blob);

    /// Fetch the str_view with the content for a specific filename
    str_view fetch_file(str_view file_name);

private:
    str_view m_tar_blob;
    size_t m_file_count;
    str_view* m_file_names;
    size_t* m_file_sizes;
    str_view* m_files;
};

} // namespace ustar