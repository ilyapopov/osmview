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

#include "tilecache.hpp"
#include "timer.hpp"

#include "sdl3.hpp"

#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include <iostream>

osmview::TileCacheItem::TileCacheItem(TileCache *cache,
                                      fs::path file_name,
                                      std::string url)
    : file_name_(std::move(file_name)), url_(std::move(url)), cache_(cache), state_(State::free),
      access_timestamp_(0u)
{
}

void osmview::TileCacheItem::load()
{
    state_ = State::loading;
    try
    {
        SDL3::Surface s(file_name_.native());

        {
            std::lock_guard<std::mutex> lock(mutex_);
            surface_ = std::move(s);
        }

        state_ = State::surface_ready;
    }
    catch (SDL3::Exception &)
    {
        state_ = State::downloading;
        cache_->download(url_, file_name_,
                         [self = shared_from_this()](bool success) {
                             return self->download_callback(success);
                         });
    }
}

void osmview::TileCacheItem::download_callback(bool success)
{
    if (success)
    {
        state_ = State::loading;
        //initiate_load();
    }
    else
    {
        state_ = State::error;
    }
}

std::optional<SDL3::Texture> &
osmview::TileCacheItem::get_texture(SDL3::Renderer &renderer, bool loading_allowed)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // Texture operations are not thread safe, thus done here
    if (!texture_)
    {
        if (!surface_)
        {
            if (!fs::exists(file_name_))
            {
                if (state_ != State::downloading)
                {
                    state_ = State::downloading;
                    cache_->download(url_, file_name_,
                                     [self = shared_from_this()](bool success) {
                                         return self->download_callback(success);
                                     });
                }
            }
            else if (loading_allowed)
            {
                try {
                    surface_ = SDL3::Surface(file_name_.native());
                } catch (SDL3::Exception& e) {
                    std::cerr << "Could not load image " << file_name_.native() << '\n';
                    throw;
                }
            }
        }

        if (surface_)
        {
            texture_.emplace(renderer, *surface_);

            // delete the surface
            surface_ = std::nullopt;
        }
    }

    return texture_;
}

std::shared_ptr<osmview::TileCacheItem>
osmview::TileCacheItem::create(osmview::TileCache *cache,
                               const fs::path &file_name,
                               const std::string &url)
{
    auto item = std::make_shared<TileCacheItem>(cache, file_name, url);
    item->initiate_load();
    return item;
}

void osmview::TileCacheItem::initiate_load()
{
    state_ = State::scheduled_for_loading;
    //cache_->schedule([self = shared_from_this()] { return self->load(); });
}
