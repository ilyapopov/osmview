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

#include <string>

#include <SDL/SDL.h>

class TileCacheItem
{
    std::string _file_name;
    std::string _url;
    bool _fetch_error;
    SDL_Surface * _surface;
    
public:

    TileCacheItem(const std::string file_name, const std::string url);
    
    ~TileCacheItem();
    
    bool fetch();
    bool download();
    bool fetch_error()
    {
        return _fetch_error;
    }
        
    SDL_Surface * get_surface() const
    {
        return _surface;
    }
};


#endif
