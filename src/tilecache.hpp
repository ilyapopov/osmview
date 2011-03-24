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

#include <map>
#include <string>

#include <SDL/SDL.h>

#include "tilecacheitem.hpp"
#include "tilefetcher.hpp"

class TileCache
{
    typedef std::string key_t;
    typedef std::map<key_t, TileCacheItem *> map_t;

    std::string _tile_dir;
    map_t _cache;
    
    static key_t make_key(int level, int i, int j);
    std::string make_file_name(int level, int i, int j);
    
    TileFetcher _fetcher;
    
public:
    TileCache(std::string tile_dir);
    ~TileCache();
    
    SDL_Surface * get_tile(int level, int i, int j);
};

#endif
