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

#include "tilecacheitem.hpp"

#include <algorithm>
#include <functional>

#include <SDL2pp/Exception.hh>

#include "tilecache.hpp"

osmview::TileCacheItem::TileCacheItem(TileCache * cache,
                                      const std::string &file_name,
                                      const std::string &url) :
    file_name_(file_name),
    url_(url),
    cache_(cache),
    state_(State::free),
    access_timestamp_(0u)
{}

void osmview::TileCacheItem::load()
{
    state_ = State::loading;
    try
    {
        SDL2pp::Surface s(file_name_);

        {
            std::lock_guard<std::mutex> lock(mutex_);
            surface_ = std::move(s);
        }

        state_ = State::surface_ready;
    }
    catch (SDL2pp::Exception &e)
    {
        state_ = State::downloading;
        cache_->download(url_, file_name_,
                         std::bind(&TileCacheItem::download_callback,
                                   shared_from_this(), std::placeholders::_1));
    }
}

void osmview::TileCacheItem::download_callback(bool success)
{
    if (success)
    {
        initiate_load();
    }
    else
    {
        state_ = State::error;
    }
}

SDL2pp::Optional<SDL2pp::Texture> &osmview::TileCacheItem::get_texture(
        SDL2pp::Renderer &renderer)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // Texture operations are not thread safe, thus done here
    if (surface_ && !texture_)
    {
        texture_.emplace(renderer, *surface_);

        // delete the surface
        surface_ = SDL2pp::NullOpt;
    }

    return texture_;
}

std::shared_ptr<osmview::TileCacheItem>
osmview::TileCacheItem::create(osmview::TileCache *cache,
                               const std::string &file_name,
                               const std::string &url)
{
    auto item = std::shared_ptr<TileCacheItem>(
                new TileCacheItem(cache, file_name, url));
    item->initiate_load();
    return item;
}

void osmview::TileCacheItem::initiate_load()
{
    state_ = State::scheduled_for_loading;
    cache_->schedule(std::bind(&TileCacheItem::load, shared_from_this()));
}
