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

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include <system_error>
#include <utility>

osmview::Downloader::Downloader(size_t nstreams)
    : idle_(nstreams)
{
}

osmview::Downloader::~Downloader()
{
    // give a change for transfers to finish
    perform();
    // stop all unfinished ones
    for (const auto& t : downloading_) {
        curl_multi_.remove(std::move(t->easy_));
    }
}

void osmview::Downloader::enqueue(const std::string& url,
    const fs::path& file_name,
    const std::function<void(bool)>& callback)
{
    auto t = std::make_unique<Task>(url, file_name, callback);
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        todo_.push(std::move(t));
    }
}

size_t osmview::Downloader::perform()
{
    curl_multi_.perform();

    std::optional<curl_multi::message> msg;

    while ((msg = curl_multi_.get_message()).has_value()) {
        // find the easy object corresponding to the finished transfer
        auto* t = static_cast<Transfer*>(msg->get_private());
        auto found = downloading_.find(t);
        // and remove it from the active transfers
        auto node = downloading_.extract(found);
        auto easy = node.value()->finalize(*msg, curl_multi_);
        // put the easy back into idle pool
        idle_.push_back(std::move(easy));
    }

    // start new transfers if any
    start_new();

    return todo_.size() + downloading_.size();
}

void osmview::Downloader::start_new()
{
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!todo_.empty() && !idle_.empty()) {
        // take one unused easy
        auto easy = std::move(idle_.back());
        idle_.pop_back();
        // take one task
        auto task = std::move(todo_.front());
        todo_.pop();
        // setup new transfer from task + easy
        auto transfer = std::make_unique<Transfer>(std::move(task), std::move(easy), curl_multi_);
        downloading_.insert(std::move(transfer));
    }
}

osmview::Downloader::Transfer::Transfer(std::unique_ptr<Task> task_ptr, curl_easy&& easy, curl_multi& multi)
    : task_(std::move(task_ptr))
    , easy_(setup(*task_, std::move(easy), multi))
{
    fs::create_directories(fs::path(task_->file_name_).parent_path());
    tmp_file_name = task_->file_name_;
    tmp_file_name += ".tmp";

    writer_.open(tmp_file_name.c_str());
}

osmview::curl_easy_in_multi osmview::Downloader::Transfer::setup(Task& task, curl_easy&& easy, curl_multi& multi)
{
    easy.set_url(task.url_.c_str())
        .set_user_agent("osmview https://github.com/ilyapopov/osmview")
        .set_write_method<FileWriter, &FileWriter::operator()>(writer_)
        .set_private(reinterpret_cast<void*>(this));
    return multi.add(std::move(easy));
}

size_t osmview::Downloader::FileWriter::operator()(std::string_view data)
{
    return std::fwrite(data.data(), 1, data.size(), file.get());
}

osmview::curl_easy osmview::Downloader::Transfer::finalize(curl_multi::message msg, curl_multi& multi)
{
    // close the file
    writer_.close();
    // move the result and call the callback
    auto easy = multi.remove(std::move(easy_));

    if (msg.success()) {
        fs::rename(tmp_file_name, task_->file_name_);
        task_->callback_(true);
    } else {
        fs::remove(tmp_file_name);
        // FIXME: do not hardcode printing, callback should take care of this
        std::cerr << "Error: downloading " << task_->url_ << " : "
                  << easy.error_message() << std::endl;
        task_->callback_(false);
    }
    return easy;
}

void osmview::Downloader::FileWriter::open(const char* path)
{
    file.reset(std::fopen(path, "wb"));
    if (!file) {
        throw std::system_error(errno, std::system_category());
    }
}
