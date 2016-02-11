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

#ifndef TILECACHE_HPP_INCLUDED
#define TILECACHE_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <SDL2pp/Optional.hh>
#include <SDL2pp/Texture.hh>

#include "downloader.hpp"
#include "worker_pool.hpp"
#include "tile_id.hpp"

namespace SDL2pp { class Font; }
namespace SDL2pp { class Renderer; }

namespace osmview
{

class TileCacheItem;

//////////////////////////////////////////////////////////////////////////////

class TileCache
{
    using key_type = TileId;
    using map_t = std::unordered_map<key_type, std::shared_ptr<TileCacheItem>>;

    std::string tile_dir_;
    std::string url_base_;
    map_t cache_;

    Downloader downloader_;
    WorkerPool<std::function<void()> > thread_pool_;

    SDL2pp::Renderer &renderer_;

    size_t max_size_;

    std::unordered_map<int, SDL2pp::Optional<SDL2pp::Texture>>
        special_tiles_;

    size_t tile_size_;
    size_t seq_;

    std::string make_file_name(key_type tile_id) const;
    std::string make_url(key_type tile_id) const;
    void generate_special_tiles();
    SDL2pp::Texture generate_text_tile(const std::string &text,
                                       SDL2pp::Font &font);
    void gc();
    TileCacheItem & get_item(key_type tile_id);

public:
    TileCache(const std::string & tile_dir, const std::string & url_base,
              SDL2pp::Renderer &renderer, size_t max_size_ = 256);
    ~TileCache();

    size_t size() const
    {
        return cache_.size();
    }

    void prefetch(key_type tile_id);
    SDL2pp::Texture & get_texture(key_type tile_id);
    
    // Following member functions are thread-safe

    template <typename Callable>
    void schedule(Callable && func)
    {
        thread_pool_.emplace(func);
    }

    template <typename Callable>
    void download(const std::string & url, const std::string & file_name,
                  Callable && callback)
    {
        downloader_.enqueue(url, file_name, callback);
    }
};

} // namespace

#endif
