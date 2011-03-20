#ifndef MAPVIEW_HPP_INCLUDED
#define MAPVIEW_HPP_INCLUDED

#include <SDL/SDL.h>

#include "tilecache.hpp"

class Mapview
{
    static const int _tile_size = 256;
    static const double _v0 = 2.0;
    static const std::string _tile_dir;
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
