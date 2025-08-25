#include <fmt/print.h>

namespace fmt::detail {
putCharFunc put_char = nullptr;
}

error
fmt::initialize(putCharFunc fn)
{
    if (fn == nullptr) {
        return ErrorCode::NULL_ARGUMENT;
    }
    detail::put_char = fn;
    return ErrorCode::SUCCESS;
}
