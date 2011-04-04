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

#ifndef MAPVIEW_HPP_INCLUDED
#define MAPVIEW_HPP_INCLUDED

#include <SDL/SDL.h>

#include "tilecache.hpp"

class Mapview
{
    static const int _tile_size = 256;
    static const double _v0 = 2.0;
    static const std::string _tile_dir;
    static const std::string _url_base;
    static const int _max_level = 15; 
    static const double _tau = 0.3;

    double _mapx;
    double _mapy;
    
    double _vx, _vy;
    double _fx, _fy;
    
    int _level;
    
    TileCache _cache;

public:

    Mapview();
    
    void center_on(double lat, double lon);
    void move(double move_x, double move_y);
    void move_pix_hard(double dx, double dy);
    void motion_step(double dt);
    
    int zoom(int step);
    
    bool render(SDL_Surface * surface);
};

#endif
