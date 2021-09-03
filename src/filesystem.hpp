#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <filesystem>

namespace osmview {

namespace fs = std::filesystem;

fs::path get_user_cache_dir();

} // namespace osmview

#endif // FILESYSTEM_HPP
