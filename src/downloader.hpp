#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <string>

namespace osmview
{

class Downloader
{
public:
    Downloader();
    ~Downloader();

    void download(const std::string &url, const std::string &file_name);
};

} // namespace

#endif // DOWNLOADER_HPP
