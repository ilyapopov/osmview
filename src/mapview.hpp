#ifndef MAPVIEW_HPP_INCLUDED
#define MAPVIEW_HPP_INCLUDED

#include <SDL/SDL.h>

#include "tilecache.hpp"

class Mapview
{
    double _lat;
    double _lon;
    
    double _vlat, _vlon;
    double _flat, _flon;
    
    int _level;
    
    TileCache _cache;

public:

    Mapview(SDL_Surface * ref_surface);
    
    void center_on(double lat, double lon);
    void move(double north, double east);
    void motion_step(double dt);
    
    int zoom(int step);
    
    bool render(SDL_Surface * surface);
};

#endif
