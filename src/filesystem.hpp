#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "boost/filesystem.hpp"

namespace osmview
{

namespace fs = boost::filesystem;

fs::path get_user_cache_dir();

} // namespace osmview

#endif // FILESYSTEM_HPP
