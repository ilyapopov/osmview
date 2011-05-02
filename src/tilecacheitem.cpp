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

#include <iostream>

#include <boost/filesystem.hpp>
#include <curl/curl.h>
#include <SDL/SDL_image.h>

#include "tilecache.hpp"

TileCacheItem::TileCacheItem(TileCache * cache, const std::string file_name, const std::string url)
:
    _file_name(file_name),
    _url(url),
    _surface(NULL),
    _busy(false),
    _cache(cache)
{
}
    
TileCacheItem::~TileCacheItem()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if(_surface != NULL)
        SDL_FreeSurface(_surface);
}

bool TileCacheItem::fetch()
{
    std::unique_lock<std::mutex> lock(_mutex);

    if(_busy)
        return true;

    _busy = true;

    lock.unlock();

    std::cout << "Fetching " << _file_name << " ..." << std::endl;
    SDL_Surface * s = IMG_Load(_file_name.c_str());

    lock.lock();

    if(s == NULL)
        _cache->request_download(this);
    else
        _surface = s;

    _busy = false;

    return true;
}

bool TileCacheItem::download()
{
    std::unique_lock<std::mutex> lock(_mutex);

    if(_busy)
        return true;

    _busy = true;

    lock.unlock();

    std::cout << "Downloading " << _url << " ..." << std::endl;
    
    boost::filesystem::path path(_file_name);
    
    boost::filesystem::create_directories(path.parent_path());

    FILE * file = fopen(path.string().c_str(), "wb");
    if(file)
    {
        CURL * curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        fclose(file);
    }
    
    lock.lock();

    _busy = false;
    
    return true;
}

SDL_Surface * TileCacheItem::get_surface_locked()
{
    _mutex.lock();
    if(_surface == NULL)
        _cache->request_fetch(this);
    return _surface;
}

void TileCacheItem::surface_unlock()
{
    _mutex.unlock();
}

