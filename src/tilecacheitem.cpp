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

#include <boost/filesystem.hpp>
#include <curl/curl.h>
#include <SDL2/SDL_image.h>

#include "tilecache.hpp"

osmview::TileCacheItem::TileCacheItem(TileCache * cache, const std::string &id, const std::string &file_name, const std::string &url)
:
    id_(id),
    file_name_(file_name),
    url_(url),
    cache_(cache)
{
    cache->request_fetch(this);
}

void osmview::TileCacheItem::fetch()
{
    SDL_Surface * s = IMG_Load(file_name_.c_str());

    std::unique_lock<std::mutex>(mutex_);

    if(s == nullptr)
    {
        cache_->request_download(this);
    }
    else
    {
        surface_.reset(s);
    }
}

void osmview::TileCacheItem::download()
{
    boost::filesystem::path path(file_name_);
    
    boost::filesystem::create_directories(path.parent_path());

    FILE * file = fopen(path.string().c_str(), "wb");
    if(file)
    {
        CURL * curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10l);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        fclose(file);
    }

    cache_->request_fetch(this);
}

SDL_Texture * osmview::TileCacheItem::get_texture(SDL_Renderer * renderer)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (texture_)
        return texture_.get();

    // Texture oprations are not thread safe, thus done here
    if (surface_)
    {
        texture_.reset(SDL_CreateTextureFromSurface(renderer, surface_.get()));
        return texture_.get();
    }

    return texture_.get();
}
