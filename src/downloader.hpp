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

#include <cstddef>
#include <functional>
#include <string>

#include "worker_pool.hpp"

namespace osmview
{

class Downloader
{
    WorkerPool<std::function<void()>> thread_pool_;
    static void download(const std::string &url, const std::string &file_name);

public:
    Downloader(size_t nstreams = 8);
    ~Downloader();

    void enqueue(const std::string &url, const std::string &file_name,
                  const std::function<void (bool)> &callback);
};

} // namespace

#endif // DOWNLOADER_HPP
