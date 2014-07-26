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

#ifndef TILECACHEITEM_HPP_INCLUDED
#define TILECACHEITEM_HPP_INCLUDED

#include <memory>
#include <mutex>
#include <string>

#include <SDL2/SDL.h>

class TileCache;

struct SurfaceDeleter
{
    void operator()(SDL_Surface * s) {SDL_FreeSurface(s);}
};

struct TextureDeleter
{
    void operator()(SDL_Texture * t) {SDL_DestroyTexture(t);}
};

class TileCacheItem
{
    const std::string id_;
    const std::string file_name_;
    const std::string url_;
    std::unique_ptr<SDL_Surface, SurfaceDeleter> surface_;
    std::unique_ptr<SDL_Texture, TextureDeleter> texture_;
    std::mutex mutex_;
    bool busy_;
    bool queued_;
    TileCache * const cache_;

public:

    TileCacheItem(TileCache * cache, const std::string & id, const std::string & file_name, const std::string & url);

    bool fetch();
    bool download();

    SDL_Texture * get_texture();

    std::string id() const
    {
        return id_;
    }
};


#endif
