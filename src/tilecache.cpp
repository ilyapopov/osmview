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

#include "tilecache.hpp"

#include <iostream>
#include <sstream>

TileCache::TileCache(const std::string tile_dir, const std::string url_base)
:
    _tile_dir(tile_dir),
    _url_base(url_base),
    _fetcher(2),
    _downloader(4)
{
}

TileCache::~TileCache()
{
    for(map_t::iterator i = _cache.begin(); i != _cache.end(); ++i)
    {
        if(i->second != NULL)
        {
            delete i->second;
            i->second = NULL;
        }
    }
}

TileCacheItem * TileCache::get_tile(int level, int i, int j)
{
    if(level < 0 )
        return NULL;
    if(i < 0 || j < 0 || i >= (1<<level) || j >= (1<<level))
        return NULL;

    key_t key = make_key(level, i, j);
    
    map_t::iterator p = _cache.find(key);

    if(p == _cache.end())
    {
        std::cout << "Miss: " << key << std::endl;
        
        std::string file_name = make_file_name(level, i, j);
        std::string url = make_url(level, i, j);
        
        TileCacheItem * item = new TileCacheItem(this, key, file_name, url);
        
        p = _cache.insert(make_pair(key, item)).first;
    }

    return p->second;
}

TileCache::key_t TileCache::make_key(int level, int i, int j)
{
    std::stringstream ss;
    ss << level << '/' << i << '/' << j;

    return ss.str();
}

std::string TileCache::make_file_name(int level, int i, int j)
{
    std::stringstream ss;
    ss << _tile_dir;
    ss << level << '/' << i << '/' << j << ".png";

    return ss.str();
}

std::string TileCache::make_url(int level, int i, int j)
{
    std::stringstream ss;
    ss << _url_base;
    ss << level << '/' << i << '/' << j << ".png";

    return ss.str();
}

void TileCache::request_fetch(TileCacheItem * item)
{
    std::cout << "Fetch request: " << item->id() << std::endl;
    _fetcher.enqueue(FetchJob(item));
}

void TileCache::request_download(TileCacheItem * item)
{
    std::cout << "Download request: " << item->id() << std::endl;
    _downloader.enqueue(DownloadJob(item));
}

void TileCache::clear_queues()
{
    _fetcher.clear();
    _downloader.clear();
}

