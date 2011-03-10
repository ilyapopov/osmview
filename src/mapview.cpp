#include "mapview.hpp"

#include <cmath>
#include <iostream>
#include <string>

#include "coord.hpp"
#include "timer.hpp"

const std::string tile_dir = "/home/ipopov/.cache/maps/tile.openstreetmap.org/";
const int max_level = 17; 

Mapview::Mapview(SDL_Surface * ref_surface)
    : _lat(0.0), _lon(0.0),   
    _vlat(0.0), _vlon(0.0), _flat(0.0), _flon(0.0),
    _level(5),
    _cache(tile_dir, ref_surface)
{
    std::cout << "Creating MapView..." << std::endl;
}

void Mapview::center_on(double lat, double lon)
{
    _lat = lat;
    _lon = lon;
    _vlat = _vlon = 0.0;
    _flat = _flon = 0.0;
}

void Mapview::move(double north, double east)
{
    double v0 = 400.0;
    _flat += v0 * north / (1<< _level);
    _flon += v0 * east / (1 << _level);
}

int Mapview::zoom(int step)
{
    _level += step;
    if(_level < 0)
        _level = 0;
    else if(_level > max_level)
        _level = max_level;
        
    return _level;
}

bool Mapview::render(SDL_Surface * surface)
{
    SDL_FillRect(surface, NULL, 0);
    
    int n = 1 << _level;

    int w = surface->w;
    int h = surface->h;

    double yc = lat2tiley(_lat, _level);
    double xc = lon2tilex(_lon, _level);
    
    const int tile_size = 256;
    
    double xmin = xc - 0.5 * w / tile_size;
    double xmax = xc + 0.5 * w / tile_size;
    double ymin = yc - 0.5 * h / tile_size;
    double ymax = yc + 0.5 * h / tile_size;
    
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
        
            int a = (int)floor((w/2) + tile_size * (i - xc));
            int b = (int)floor((h/2) + tile_size * (j - yc));
            
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
    double tau = 0.3;
    _vlat += (_flat - _vlat)*dt/tau;
    _vlon += (_flon - _vlon)*dt/tau;
    
    _flat = 0;
    _flon = 0;

    _lat += _vlat * dt;
    if(_lat < -85.0)
        _lat = -85.0;
    else if(_lat > 85.0)
        _lat = 85.0;

    _lon += _vlon * dt;
    if(_lon < -180.0)
        _lon += 360.0; 
    else if(_lon > 180.0)
        _lon -= 360.0;
}
