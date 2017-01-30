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

#include <cerrno>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <system_error>

#include <boost/filesystem.hpp>

osmview::Downloader::Downloader(size_t nstreams) : idle_(nstreams)
{
}

osmview::Downloader::~Downloader()
{
    for (auto &transfer : active_)
    {
        curl_multi_.remove(transfer.easy);
    }
}

void osmview::Downloader::enqueue(const std::string &url,
                                  const std::string &file_name,
                                  const std::function<void(bool)> &callback)
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    queue_.push({url, file_name, callback});
}

void osmview::Downloader::perform()
{
    curl_multi_.perform();

    CURLMsg *message;
    while ((message = curl_multi_.info_read()) != nullptr)
    {
        if (message->msg != CURLMSG_DONE)
            continue;

        CURLcode result = message->data.result;

        // find the easy object corresponding to the finished transfer
        auto found =
            std::find_if(active_.begin(), active_.end(), [&](Transfer &item) {
                return item.easy.handle() == message->easy_handle;
            });
        // and remove it from the active transfers
        auto item = std::move(*found);
        active_.erase(found);
        curl_multi_.remove(item.easy);

        // process the finished transfer
        item.finalize(result);

        // put the transfer into unused pool
        idle_.push_back(std::move(item));
    }

    // start new transfers if any
    start_new();
}

void osmview::Downloader::start_new()
{
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!queue_.empty() && !idle_.empty())
    {
        // take one unused easy
        auto item = std::move(idle_.back());
        idle_.pop_back();
        // setup new transfer
        item.setup(std::move(queue_.front()));
        queue_.pop();
        // submit it
        curl_multi_.add(item.easy);
        active_.push_back(std::move(item));
    }
}

void osmview::Downloader::Transfer::setup(osmview::Downloader::Task &&q)
{
    namespace fs = boost::filesystem;

    task = std::move(q);

    fs::create_directories(fs::path(task.file_name).parent_path());
    tmp_file_name = task.file_name + ".tmp";

    file.reset(std::fopen(tmp_file_name.c_str(), "wb"));

    if (!file)
    {
        throw std::system_error(errno, std::system_category());
    }

    easy.setup_download(task.url.c_str(), write_callback, file.get());
}

void osmview::Downloader::Transfer::finalize(CURLcode code)
{
    namespace fs = boost::filesystem;

    // close the file
    file.reset();
    // move the result and call the callback
    if (code == CURLE_OK)
    {
        fs::rename(tmp_file_name, task.file_name);
        task.callback(true);
    }
    else
    {
        fs::remove(tmp_file_name);
        std::cerr << "Error: downloading " << task.url << " (" << code << "): "
                  << easy.error_message(code) << std::endl;
        task.callback(false);
    }
}

size_t osmview::Downloader::Transfer::write_callback(char *ptr, size_t size,
                                                     size_t nmemb,
                                                     void *userdata)
{
    return std::fwrite(ptr, size, nmemb, reinterpret_cast<FILE *>(userdata));
}
