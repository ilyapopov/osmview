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

#ifndef TILECACHE_HPP_INCLUDED
#define TILECACHE_HPP_INCLUDED

#include <unordered_map>
#include <string>

#include <SDL2/SDL_render.h>

#include "worker.hpp"
#include "tilecacheitem.hpp"

class FetchJob
{
    TileCacheItem * item_;

public:
    FetchJob(TileCacheItem * item)
    : item_(item)
    {}

    void operator()()
    {
        item_->fetch();
    }
};

//////////////////////////////////////////////////////////////////////////////

class DownloadJob
{
    TileCacheItem * item_;

public:
    DownloadJob(TileCacheItem * item)
    : item_(item)
    {}

    void operator()()
    {
        item_->download();
    }
};

//////////////////////////////////////////////////////////////////////////////

class TileCache
{
    typedef std::string key_t;
    typedef std::unordered_map<key_t, TileCacheItem *> map_t;

    std::string tile_dir_;
    std::string url_base_;
    map_t cache_;
    
    static key_t make_key(int level, int i, int j);
    std::string make_file_name(int level, int i, int j);
    std::string make_url(int level, int i, int j);
    
    WorkerPool<FetchJob> fetcher_;
    WorkerPool<DownloadJob> downloader_;

    SDL_Renderer * renderer_;
    
public:
    TileCache(const std::string & tile_dir, const std::string & url_base, SDL_Renderer * renderer);
    ~TileCache();
    
    SDL_Texture * get_texture(int level, int i, int j);
    
    void request_fetch(TileCacheItem * item);
    void request_download(TileCacheItem * item);

    void clear_queues();

    SDL_Renderer * renderer()
    {
        return renderer_;
    }
};

#endif
