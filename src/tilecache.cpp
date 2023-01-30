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

#include "tilecacheitem.hpp"
#include "timer.hpp"

#include "SDL.h"
#include "SDL2pp/Font.hh"
#include "SDL2pp/Point.hh"
#include "SDL2pp/Surface.hh"

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>


osmview::TileCache::TileCache(fs::path tile_dir,
                              std::string url_base,
                              SDL2pp::Renderer &renderer,
                              size_t max_size) :
    tile_dir_(std::move(tile_dir)),
    url_base_(std::move(url_base)),
    renderer_(renderer),
    max_size_(max_size),
    tile_size_(256),
    seq_(0)
{
    generate_special_tiles();
}

osmview::TileCache::~TileCache() = default;

osmview::TileCacheItem &osmview::TileCache::get_item(key_type tile_id)
{
    auto p = cache_.find(tile_id);

    if (p == cache_.end())
    {
        if (size() >= max_size_)
        {
            gc();
        }

        fs::path file_name = make_file_name(tile_id);
        std::string url = make_url(tile_id);

        auto item = TileCacheItem::create(this, file_name, url);

        p = cache_.emplace(tile_id, item).first;
    }

    return *(p->second);
}

void osmview::TileCache::prefetch(key_type tile_id)
{
    auto & item = get_item(tile_id);
    item.set_access_timestamp(seq_++);
}

SDL2pp::Texture & osmview::TileCache::get_texture(key_type tile_id, bool loading_allowed)
{
    downloader_.perform();

    auto & item = get_item(tile_id);
    item.set_access_timestamp(seq_++);

    auto & texture = item.get_texture(renderer_, loading_allowed);

    if (texture)
    {
        return *texture;
    }

    return *special_tiles_.at(item.state());
}

void osmview::TileCache::gc()
{
    std::vector<std::pair<size_t, key_type>> timestamps;
    timestamps.reserve(cache_.size());

    for (const auto & item: cache_)
    {
        timestamps.emplace_back(item.second->access_timestamp(), item.first);
    }

    // will be removing the oldert 1/4 of the elements
    auto mid = timestamps.begin() + timestamps.size()/4;
    // we don't need the full sort here. nth_element guarantees that all elements before mid are smaller than mid.
    std::nth_element(timestamps.begin(), mid, timestamps.end());

    for (auto i = timestamps.begin(); i != mid; ++i)
    {
        cache_.erase(i->second);
    }
}

osmview::fs::path osmview::TileCache::make_file_name(key_type tile_id) const
{
    osmview::fs::path result = tile_dir_;
    result /= std::to_string(tile_id.level());
    result /= std::to_string(tile_id.x());
    result /= std::to_string(tile_id.y()) + ".png";
    return result;
}

std::string osmview::TileCache::make_url(key_type tile_id) const
{
    std::ostringstream ss;
    ss << url_base_ << '/'
       << tile_id.level() << '/'
       << tile_id.x() << '/'
       << tile_id.y() << ".png";
    return ss.str();
}

void osmview::TileCache::generate_special_tiles()
{
    SDL2pp::Font font("data/DejaVuSans.ttf", 20);
    special_tiles_.emplace(TileCacheItem::State::loading,
                           generate_text_tile("Loading...", font));
    special_tiles_.emplace(TileCacheItem::State::downloading,
                           generate_text_tile("Downoading...", font));
    special_tiles_.emplace(TileCacheItem::State::error,
                           generate_text_tile("Error", font));
    special_tiles_.emplace(TileCacheItem::State::scheduled_for_loading,
                           generate_text_tile("Scheduled", font));
    special_tiles_.emplace(TileCacheItem::State::scheduled_for_downloading,
                           generate_text_tile("Scheduled", font));
}

SDL2pp::Texture
osmview::TileCache::generate_text_tile(const std::string &text,
                                       SDL2pp::Font &font)
{
    SDL2pp::Surface surface(0, tile_size_, tile_size_, 32, 0u, 0u, 0u, 0u);
    surface.FillRect(SDL2pp::NullOpt, SDL_MapRGB(surface.Get()->format, 192, 192, 192));

    auto text_surface = font.RenderText_Blended(text, {128, 128, 128, 0});
    text_surface.Blit(SDL2pp::NullOpt, surface,
    {(surface.GetSize() - text_surface.GetSize())/ 2, text_surface.GetSize()});

    return SDL2pp::Texture(renderer_, surface);
}
