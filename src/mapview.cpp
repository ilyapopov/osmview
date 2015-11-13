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

#include "mapview.hpp"

#include <cmath>
#include <string>

#include "coord.hpp"

const std::string osmview::Mapview::tile_dir_("/home/ipopov/.cache/maps/tile.openstreetmap.org/");
const std::string osmview::Mapview::url_base_("http://tile.openstreetmap.org/");
const double osmview::Mapview::v0_ = 2.0;
const double osmview::Mapview::tau_ = 0.3;

osmview::Mapview::Mapview(SDL_Renderer * renderer)
    : mapx_(0.5), mapy_(0.5),
    vx_(0.0), vy_(0.0), fx_(0.0), fy_(0.0),
    level_(1),
    cache_(tile_dir_, url_base_, renderer),
    renderer_(renderer)
{
}

void osmview::Mapview::center_on(double lat, double lon)
{
    mapx_ = osmview::lon2mapx(lon);
    mapy_ = osmview::lat2mapy(lat);
    vx_ = vy_ = 0.0;
    fx_ = fy_ = 0.0;
}

void osmview::Mapview::move(double move_x, double move_y)
{
    double scale = pow(2.0, level_);
    fx_ += v0_ * move_x / scale;
    fy_ += v0_ * move_y / scale;
}

void osmview::Mapview::move_pix_hard(double dx, double dy)
{
    double scale = pow(2.0, level_);
    mapx_ += dx / tile_size_ / scale;
    mapy_ += dy / tile_size_ / scale;
}

int osmview::Mapview::zoom(int step)
{
    level_ += step;
    if(level_ < 0)
        level_ = 0;
    else if(level_ > max_level_)
        level_ = max_level_;
        
    return level_;
}

bool osmview::Mapview::render()
{
    SDL_RenderClear(renderer_);
    
    int n = 1 << level_;

    int w = 0;
    int h = 0;

    SDL_GetRendererOutputSize(renderer_, &w, &h);

    double xc = mapx_ * n;
    double yc = mapy_ * n;
    
    double xmin = xc - 0.5 * w / tile_size_;
    double xmax = xc + 0.5 * w / tile_size_;
    double ymin = yc - 0.5 * h / tile_size_;
    double ymax = yc + 0.5 * h / tile_size_;
    
    int imin = floor(xmin);
    int imax = ceil(xmax);
    int jmin = floor(ymin);
    int jmax = ceil(ymax);

    for(int i = imin; i < imax; ++i)
    {
        for(int j = jmin; j < jmax; ++j)
        {
            if(j < 0 || j >= n)
            {
                continue;
            }
            
            int i1 = (i < 0) ? (i + n) : ((i >= n) ? (i-n) : i);
            
            SDL_Texture * tile = cache_.get_texture(level_, i1, j);
            if (tile == nullptr)
            {
                continue;
            }

            int a = floor((w/2) + tile_size_ * (i - xc));
            int b = floor((h/2) + tile_size_ * (j - yc));
                
            SDL_Rect rect = {a, b, tile_size_, tile_size_};
            SDL_RenderCopy(renderer_, tile, nullptr, &rect);
        }
    }
    
    return true;
}

void osmview::Mapview::motion_step(double dt)
{
    vx_ += (fx_ - vx_)*dt/tau_;
    vy_ += (fy_ - vy_)*dt/tau_;
    
    fx_ = 0.0;
    fy_ = 0.0;

    mapx_ = wrap(mapx_ + vx_ * dt, 0.0, 1.0);
    mapy_ = clamp(mapy_ + vy_ * dt, 0.0, 1.0);
}
