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
#include "timer.hpp"

const std::string Mapview::_tile_dir("/home/ipopov/.cache/maps/tile.openstreetmap.org/");

Mapview::Mapview()
    : _mapx(0.5), _mapy(0.5),   
    _vx(0.0), _vy(0.0), _fx(0.0), _fy(0.0),
    _level(5),
    _cache(_tile_dir)
{
}

void Mapview::center_on(double lat, double lon)
{
    _mapx = lon2mapx(lon);
    _mapy = lat2mapy(lat);
    _vx = _vy = 0.0;
    _fx = _fy = 0.0;
}

void Mapview::move(double move_x, double move_y)
{
    double scale = pow(2.0, _level);
    _fx += _v0 * move_x / scale;
    _fy += _v0 * move_y / scale;
}

void Mapview::move_pix_hard(double dx, double dy)
{
    double scale = pow(2.0, _level);
    _mapx += dx / _tile_size / scale;
    _mapy += dy / _tile_size / scale;
}

int Mapview::zoom(int step)
{
    _level += step;
    if(_level < 0)
        _level = 0;
    else if(_level > _max_level)
        _level = _max_level;
        
    return _level;
}

bool Mapview::render(SDL_Surface * surface)
{
    SDL_FillRect(surface, NULL, 0);
    
    int n = 1 << _level;

    int w = surface->w;
    int h = surface->h;

    double xc = _mapx * n;
    double yc = _mapy * n;
    
    double xmin = xc - 0.5 * w / _tile_size;
    double xmax = xc + 0.5 * w / _tile_size;
    double ymin = yc - 0.5 * h / _tile_size;
    double ymax = yc + 0.5 * h / _tile_size;
    
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
        
            int a = static_cast<int>(floor((w/2) + _tile_size * (i - xc)));
            int b = static_cast<int>(floor((h/2) + _tile_size * (j - yc)));
            
            int i1 = (i < 0) ? (i + n) : ((i >= n) ? (i-n) : i);
            
            SDL_Surface * tile = _cache.get_tile(_level, i1, j);
            if(tile == NULL)
                continue;
                
            SDL_Rect rect = {a, b, 0, 0};
            SDL_BlitSurface(tile, NULL, surface, &rect);
        }
    }
    
    return true;
}

void Mapview::motion_step(double dt)
{
    _vx += (_fx - _vx)*dt/_tau;
    _vy += (_fy - _vy)*dt/_tau;
    
    _fx = 0.0;
    _fy = 0.0;

    _mapy += _vy * dt;
    if(_mapy < 0.0)
        _mapy = 0.0;
    else if(_mapy > 1.0)
        _mapy = 1.0;

    _mapx += _vx * dt;
    if(_mapx < 0.0)
        _mapx += 1.0; 
    else if(_mapx > 1.0)
        _mapx -= 1.0;
}
