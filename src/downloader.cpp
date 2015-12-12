#include "downloader.hpp"

#include <cstdio>
#include <memory>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <curl/curl.h>

osmview::Downloader::Downloader()
{
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
    {
        throw std::runtime_error("Cannot initialize libCURL");
    }
}

osmview::Downloader::~Downloader()
{
    curl_global_cleanup();
}

void osmview::Downloader::download(const std::string &url, const std::string &file_name)
{
    namespace fs = boost::filesystem;

    fs::create_directories(fs::path(file_name).parent_path());

    std::string tmp_file_name = file_name + ".tmp";

    {
        std::unique_ptr<FILE, int (*)(FILE*)>
                file(std::fopen(tmp_file_name.c_str(), "wb"), std::fclose);
        if (!file)
        {
            throw std::system_error(errno, std::system_category());
        }

        std::unique_ptr<CURL, void (*)(CURL*)>
                curl(curl_easy_init(), curl_easy_cleanup);
        if (!curl)
        {
            throw std::runtime_error("Cannot initialize CURL easy handle");
        }

        char errorrbuf[CURL_ERROR_SIZE];
        curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, errorrbuf);
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, file.get());
        curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 10l);
        if (curl_easy_perform(curl.get()) != CURLE_OK)
        {
            throw std::runtime_error(errorrbuf);
        }
    }

    fs::rename(tmp_file_name, file_name);
}

