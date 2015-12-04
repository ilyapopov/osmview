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

#include <functional>
#include <string>
#include <unordered_map>

#include "worker.hpp"

#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

namespace osmview
{

class TileCacheItem;

//////////////////////////////////////////////////////////////////////////////

class TileCache
{
    typedef std::string key_t;
    typedef std::unordered_map<key_t, std::unique_ptr<TileCacheItem>> map_t;

    std::string tile_dir_;
    std::string url_base_;
    map_t cache_;
    
    WorkerPool<std::function<void()> > fetcher_;
    WorkerPool<std::function<void()> > downloader_;

    SDL2pp::Renderer &renderer_;

    SDL2pp::Texture loading_texture_;

    size_t max_size_;
    
    static key_t make_key(int level, int i, int j);
    std::string make_file_name(int level, int i, int j) const;
    std::string make_url(int level, int i, int j) const;
    SDL2pp::Texture generate_loading_texture();
    SDL2pp::Texture generate_downloading_texture();
    void gc();

public:
    TileCache(const std::string & tile_dir, const std::string & url_base,
              SDL2pp::Renderer &renderer, size_t max_size_ = 256);
    ~TileCache();

    size_t size() const
    {
        return cache_.size();
    }

    SDL2pp::Texture & get_texture(int level, int i, int j, size_t timestamp);
    
    void request_load(TileCacheItem * item);
    void request_download(TileCacheItem * item);
};

}

#endif
