#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#ifdef USE_STD_FILESYSTEM

#include <filesystem>

namespace osmview {
namespace fs = std::filesystem;
}

#else

#include <boost/filesystem.hpp>

namespace osmview {
namespace fs = boost::filesystem;
}

#endif

namespace osmview {

fs::path get_user_cache_dir();

} // namespace osmview

#endif // FILESYSTEM_HPP
