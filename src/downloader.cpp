/*
    Copyright 2011, 2014, 2015, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.
    https://bitbucket.org/ipopov/osmview

    osmview is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    osmview is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with osmview.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "downloader.hpp"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <curl/curl.h>

osmview::Downloader::Downloader(size_t nstreams) :
    thread_pool_(nstreams)
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

void osmview::Downloader::enqueue(const std::string &url,
                                   const std::string &file_name,
                                   const std::function<void (bool)> &callback)
{
    thread_pool_.emplace([url, file_name, callback]{
        try
        {
            download(url, file_name);
        }
        catch (std::exception & e)
        {
            std::cerr << "Error downloading from " << url << std::endl;
            std::cerr << e.what() << std::endl;

            callback(false);
        }

        callback(true);
    });
}

