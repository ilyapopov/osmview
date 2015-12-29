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

#include "tilecache.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <SDL2pp/Surface.hh>

#include "tilecacheitem.hpp"

osmview::TileCache::TileCache(const std::string &tile_dir,
                              const std::string &url_base,
                              SDL2pp::Renderer &renderer,
                              size_t max_size) :
    tile_dir_(tile_dir),
    url_base_(url_base),
    fetcher_pool_(std::thread::hardware_concurrency()),
    downloader_pool_(8),
    renderer_(renderer),
    max_size_(max_size)
{
    generate_special_tiles();
}

osmview::TileCache::~TileCache()
{}

osmview::TileCacheItem &osmview::TileCache::get_item(key_type tile_id)
{
    auto p = cache_.find(tile_id);

    if (p == cache_.end())
    {
        if (size() >= max_size_)
        {
            gc();
        }

        std::string file_name = make_file_name(tile_id);
        std::string url = make_url(tile_id);

        auto item = std::make_shared<TileCacheItem>(this, file_name, url);
        request_load(item);

        p = cache_.emplace(tile_id, item).first;
    }

    return *(p->second);
}

void osmview::TileCache::prefetch(key_type tile_id, size_t timestamp)
{
    auto & item = get_item(tile_id);
    item.set_timestamp(timestamp);
}

SDL2pp::Texture & osmview::TileCache::get_texture(key_type tile_id,
                                                  size_t timestamp)
{
    auto & item = get_item(tile_id);
    item.set_timestamp(timestamp);

    auto & texture = item.get_texture(renderer_);

    if (texture)
        return *texture;

    return *special_tiles_.at((int)item.state());
}

void osmview::TileCache::gc()
{
    std::vector<std::pair<size_t, key_type>> timestamps;
    timestamps.reserve(cache_.size());

    for (const auto & item: cache_)
    {
        timestamps.emplace_back(item.second->access_timestamp(), item.first);
    }

    std::sort(timestamps.begin(), timestamps.end());

    for (auto i = timestamps.begin();
         i != timestamps.begin() + timestamps.size()/4; ++i)
    {
        cache_.erase(i->second);
    }
}



std::string osmview::TileCache::make_file_name(key_type tile_id) const
{
    std::ostringstream ss;
    ss << tile_dir_ << '/' << tile_id << ".png";
    return ss.str();
}

std::string osmview::TileCache::make_url(key_type tile_id) const
{
    std::ostringstream ss;
    ss << url_base_ << '/' << tile_id << ".png";
    return ss.str();
}

void osmview::TileCache::generate_special_tiles()
{
    SDL2pp::Font font("data/ClearSans-Medium.ttf", 24);
    special_tiles_.emplace((int)TileCacheItem::state_t::loading,
                           generate_text_tile("Loading...", font));
    special_tiles_.emplace((int)TileCacheItem::state_t::downloading,
                           generate_text_tile("Downoading...", font));
    special_tiles_.emplace((int)TileCacheItem::state_t::error,
                           generate_text_tile("Error", font));
    special_tiles_.emplace((int)TileCacheItem::state_t::scheduled_for_loading,
                           generate_text_tile("Scheduled", font));
    special_tiles_.emplace((int)TileCacheItem::state_t::scheduled_for_downloading,
                           generate_text_tile("Scheduled", font));
}

SDL2pp::Texture osmview::TileCache::generate_text_tile(const std::string &text, SDL2pp::Font &font)
{
    SDL2pp::Surface tile(0, 256, 256, 32, 0, 0, 0, 0);
    tile.FillRect(SDL2pp::NullOpt, 0x00000000);

    auto text_surface = font.RenderText_Blended(text, {128, 128, 128, 0});
    text_surface.Blit(SDL2pp::NullOpt, tile,
    {(tile.GetSize() - text_surface.GetSize())/ 2, text_surface.GetSize()});

    return SDL2pp::Texture(renderer_, tile);
}

void osmview::TileCache::request_load(std::shared_ptr<TileCacheItem> item)
{
    fetcher_pool_.emplace([item](){if (item.use_count() > 1) item->load();});
}

void osmview::TileCache::request_download(std::shared_ptr<TileCacheItem> item)
{
    downloader_pool_.emplace([item](){if (item.use_count() > 1) item->download();});
}
