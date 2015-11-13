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

#include <string>

#include <SDL2/SDL.h>

#include "tilecache.hpp"

namespace osmview
{

class Mapview
{
    static const int tile_size_ = 256;
    static const int max_level_ = 18;
    static const std::string tile_dir_;
    static const std::string url_base_;

    static const double v0_;
    static const double tau_;

    double mapx_;
    double mapy_;
    
    double vx_, vy_;
    double fx_, fy_;
    
    int level_;
    
    TileCache cache_;

    SDL_Renderer * renderer_;

public:

    Mapview(SDL_Renderer * renderer);
    
    void center_on(double lat, double lon);
    void move(double move_x, double move_y);
    void move_pix_hard(double dx, double dy);
    void motion_step(double dt);
    
    int zoom(int step);
    
    bool render();
};

}

#endif
