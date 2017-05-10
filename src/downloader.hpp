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

#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include "curl.hpp"
#include "filesystem.hpp"

#include "curl/curl.h"

#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

namespace osmview
{

class Downloader
{
    struct Task
    {
        std::string url;
        fs::path file_name;
        std::function<void(bool)> callback;
    };

    struct Transfer
    {
        curl_easy easy;
        Task task;

        struct file_closer
        {
            void operator()(FILE *f) const { std::fclose(f); }
        };
        std::unique_ptr<FILE, file_closer> file;

        fs::path tmp_file_name;

        void setup(Task &&q);
        void finalize(CURLcode code);

        static size_t write_callback(char *ptr, size_t size, size_t nmemb,
                                     void *userdata);
    };

    curl_global curl_global_;
    std::queue<Task> queue_;
    std::mutex queue_mutex_;
    curl_multi curl_multi_;
    std::vector<Transfer> idle_;
    std::vector<Transfer> active_;

    void start_new();

public:
    Downloader(size_t nstreams = 8);
    ~Downloader();

    void enqueue(const std::string &url, const fs::path &file_name,
                 const std::function<void(bool)> &callback);
    void perform();
};

} // namespace

#endif // DOWNLOADER_HPP
