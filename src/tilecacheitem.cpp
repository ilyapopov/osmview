/*
    Copyright 2011, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.

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

#include "tilecacheitem.hpp"

#include <cstdio>
#include <iostream>

#include <boost/filesystem.hpp>
#include <curl/curl.h>
#include <SDL2/SDL_image.h>

#include "tilecache.hpp"

TileCacheItem::TileCacheItem(TileCache * cache, const std::string &id, const std::string &file_name, const std::string &url)
:
    id_(id),
    file_name_(file_name),
    url_(url),
    busy_(false),
    queued_(false),
    cache_(cache)
{}

bool TileCacheItem::fetch()
{
    std::unique_lock<std::mutex> lock(mutex_);

    if(busy_)
        return true;

    busy_ = true;

    lock.unlock();

    SDL_Surface * s = IMG_Load(file_name_.c_str());

    lock.lock();

    if(s == nullptr)
    {
        cache_->request_download(this);
    }
    else
    {
        surface_.reset(s);
        queued_ = false;
    }

    busy_ = false;

    return true;
}

bool TileCacheItem::download()
{
    std::unique_lock<std::mutex> lock(mutex_);

    if(busy_)
        return true;

    busy_ = true;

    lock.unlock();

    boost::filesystem::path path(file_name_);
    
    boost::filesystem::create_directories(path.parent_path());

    FILE * file = fopen(path.string().c_str(), "wb");
    if(file)
    {
        CURL * curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        fclose(file);
    }
    
    lock.lock();

    busy_ = false;
    queued_ = false;
    
    return true;
}

SDL_Texture * TileCacheItem::get_texture()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!surface_ && !queued_ && !busy_)
    {
        cache_->request_fetch(this);
        queued_ = true;
    }

    // Texture oprations are not thread safe, thus done here
    if (surface_ && !texture_)
    {
        texture_.reset(SDL_CreateTextureFromSurface(cache_->renderer(), surface_.get()));
        if (!texture_)
            std::cerr << "Texture creation failed " << id_ << std::endl;
    }

    return texture_.get();
}
