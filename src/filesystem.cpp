#include "filesystem.hpp"

#include <cstdlib>
#include <stdexcept>

osmview::fs::path osmview::get_user_cache_dir()
{
    const char * d = nullptr;
    // Using "XDG Base Directory Specification"
    if ((d = std::getenv("XDG_CACHE_HOME")) != nullptr)
    {
        return d;
    }
    // Use default directory
    if ((d = std::getenv("HOME")) != nullptr)
    {
        return fs::path(d) / ".cache";
    }
    throw std::runtime_error("Cannot figure out cache directory location");
}
