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

#include <SDL/SDL_image.h>

TileCacheItem::TileCacheItem(std::string file_name)
: _file_name(file_name), _surface(NULL)
{
}
    
TileCacheItem::~TileCacheItem()
{
    if(_surface != NULL)
        SDL_FreeSurface(_surface);
}

bool TileCacheItem::fetch()
{
    if(_surface != NULL)
        SDL_FreeSurface(_surface);
    SDL_Surface * s = IMG_Load(_file_name.c_str());
    if(s == NULL)
        return false;
    _surface = SDL_DisplayFormat(s);
    return true;
}
