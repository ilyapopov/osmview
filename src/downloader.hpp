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

#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <string>
#include <vector>

namespace osmview {

struct unique_ptr_cmp {
    template <typename T>
    bool operator()(const std::unique_ptr<T>& lhs, const T* rhs) const
    {
        return lhs.get() < rhs;
    }
    template <typename T>
    bool operator()(const T* lhs, const std::unique_ptr<T>& rhs) const
    {
        return lhs < rhs.get();
    }
    template <typename T>
    bool operator()(const std::unique_ptr<T>& lhs, const std::unique_ptr<T>& rhs) const
    {
        return lhs < rhs;
    }

    using is_transparent = int;
};

class Downloader {
    struct Task {
        std::string url_;
        fs::path file_name_;
        std::function<void(bool)> callback_;
        Task(std::string url, fs::path filename, std::function<void(bool)> callback)
            : url_(std::move(url))
            , file_name_(std::move(filename))
            , callback_(std::move(callback))
        {
        }
    };

    struct FileWriter {
        struct file_closer {
            void operator()(FILE* f) const { std::fclose(f); }
        };
        std::unique_ptr<FILE, file_closer> file = nullptr;

        void open(const char* path);
        void close() { file.reset(); };
        size_t operator()(std::string_view data);
    };

    struct Transfer {
        std::unique_ptr<Task> task_;
        curl_easy_handle easy_;
        fs::path tmp_file_name;
        FileWriter writer_;

        Transfer(std::unique_ptr<Task> task_ptr, curl_easy&& easy, curl_multi& multi);
        curl_easy_handle setup(Task& task, curl_easy&& easy, curl_multi& multi);
        osmview::curl_easy finalize(curl_multi::message msg, curl_multi& multi);
    };

    curl_global curl_global_;
    std::mutex queue_mutex_;
    std::queue<std::unique_ptr<Task>> todo_;
    std::set<std::unique_ptr<Transfer>, unique_ptr_cmp> downloading_;
    //std::queue<std::unique_ptr<DownloadResult>> done_;
    std::vector<curl_easy> idle_;

    curl_multi curl_multi_;

    void start_new();

public:
    explicit Downloader(size_t nstreams = 8);
    ~Downloader();

    void enqueue(const std::string& url, const fs::path& file_name,
        const std::function<void(bool)>& callback);
    size_t perform();
};

} // namespace osmview

#endif // DOWNLOADER_HPP
