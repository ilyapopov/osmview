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

#include <stdexcept>

#include <SDL2pp/Exception.hh>

#include "tilecache.hpp"

osmview::TileCacheItem::TileCacheItem(TileCache * cache, const std::string &id,
                                      const std::string &file_name,
                                      const std::string &url) :
    id_(id),
    file_name_(file_name),
    url_(url),
    cache_(cache),
    state_(state_t::free)
{
    state_ = state_t::scheduled_for_loading;
    cache->request_load(this);
}

void osmview::TileCacheItem::load()
{
    assert(state_ == state_t::scheduled_for_loading);
    state_ = state_t::loading;
    try
    {
        SDL2pp::Surface s(file_name_);

        {
            std::unique_lock<std::mutex>(mutex_);
            surface_ = std::move(s);
        }

        state_ = state_t::surface_ready;
    }
    catch (SDL2pp::Exception &e)
    {
        state_ = state_t::scheduled_for_downloading;
        cache_->request_download(this);
    }
}

void osmview::TileCacheItem::download()
{
    assert(state_ == state_t::scheduled_for_downloading);
    state_ = state_t::downloading;
    try
    {
        cache_->download(url_, file_name_);

        state_ = state_t::scheduled_for_loading;
        cache_->request_load(this);
    }
    catch (std::exception & e)
    {
        state_ = state_t::error;
        std::cerr << "Error downloading from " << url_ << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

SDL2pp::Optional<SDL2pp::Texture> &osmview::TileCacheItem::get_texture(
        SDL2pp::Renderer &renderer, size_t timestamp)
{
    std::lock_guard<std::mutex> lock(mutex_);

    last_access_timestamp_ = timestamp;

    // Texture operations are not thread safe, thus done here
    if (surface_ && !texture_)
    {
        texture_ = SDL2pp::Texture(renderer, *surface_);

        // delete the surface
        surface_ = SDL2pp::NullOpt;
    }


    return texture_;
}
