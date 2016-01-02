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

#ifndef TILECACHEITEM_HPP_INCLUDED
#define TILECACHEITEM_HPP_INCLUDED

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include <SDL2pp/Optional.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Surface.hh>
#include <SDL2pp/Texture.hh>

namespace osmview
{

class TileCache;

class TileCacheItem : public std::enable_shared_from_this<TileCacheItem>
{
public:
    enum class State
    {
        free = 0,
        scheduled_for_loading = 1,
        loading = 2,
        scheduled_for_downloading = 3,
        downloading = 4,
        surface_ready = 5,
        error = 6
    };

private:
    const std::string file_name_;
    const std::string url_;
    SDL2pp::Optional<SDL2pp::Surface> surface_;
    SDL2pp::Optional<SDL2pp::Texture> texture_;
    std::mutex mutex_;
    TileCache * cache_;
    std::atomic<State> state_;
    size_t access_timestamp_;

    TileCacheItem(TileCache * cache,
                  const std::string & file_name, const std::string & url);

    void load();
    void download_callback(bool);
    void initiate_load();

public:

    SDL2pp::Optional<SDL2pp::Texture> &get_texture(SDL2pp::Renderer &renderer);

    State state() const
    {
        return state_;
    }

    std::size_t access_timestamp() const
    {
        return access_timestamp_;
    }

    void set_access_timestamp(size_t timestamp)
    {
        access_timestamp_ = timestamp;
    }

    static std::shared_ptr<TileCacheItem>
    create(TileCache * cache, const std::string & file_name,
           const std::string & url);

};

}

#endif
