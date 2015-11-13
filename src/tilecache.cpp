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

#include <sstream>
#include <utility>

#include "tilecacheitem.hpp"

osmview::TileCache::TileCache(const std::string &tile_dir, const std::string &url_base, SDL_Renderer *renderer)
    :
    tile_dir_(tile_dir),
    url_base_(url_base),
    fetcher_(std::thread::hardware_concurrency()),
    downloader_(16),
    renderer_(renderer)
{}

osmview::TileCache::~TileCache()
{}

SDL_Texture * osmview::TileCache::get_texture(int level, int i, int j)
{
    if (level < 0 )
        return nullptr;
    if (i < 0 || j < 0 || i >= (1<<level) || j >= (1<<level))
        return nullptr;

    key_t key = make_key(level, i, j);
    
    map_t::iterator p = cache_.find(key);

    if (p == cache_.end())
    {
        std::string file_name = make_file_name(level, i, j);
        std::string url = make_url(level, i, j);
        
        std::unique_ptr<TileCacheItem> tci(new TileCacheItem(this, key, file_name, url));
        p = cache_.emplace(key, std::move(tci)).first;
    }

    return p->second->get_texture(renderer_);
}

osmview::TileCache::key_t osmview::TileCache::make_key(int level, int i, int j)
{
    std::ostringstream ss;
    ss << level << '/' << i << '/' << j;

    return ss.str();
}

std::string osmview::TileCache::make_file_name(int level, int i, int j) const
{
    std::ostringstream ss;
    ss << tile_dir_;
    ss << level << '/' << i << '/' << j << ".png";

    return ss.str();
}

std::string osmview::TileCache::make_url(int level, int i, int j) const
{
    std::ostringstream ss;
    ss << url_base_;
    ss << level << '/' << i << '/' << j << ".png";

    return ss.str();
}

void osmview::TileCache::request_fetch(TileCacheItem * item)
{
    fetcher_.emplace([item](){item->fetch();});
}

void osmview::TileCache::request_download(TileCacheItem * item)
{
    downloader_.emplace([item](){item->download();});
}
