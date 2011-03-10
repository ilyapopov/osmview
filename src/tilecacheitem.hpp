#ifndef TILECACHEITEM_HPP_INCLUDED
#define TILECACHEITEM_HPP_INCLUDED

#include <string>

#include <SDL/SDL.h>

class TileCacheItem
{
    std::string _file_name;
    SDL_Surface * _surface;
    
public:

    TileCacheItem(std::string file_name);
    
    ~TileCacheItem();
    
    bool fetch();
        
    SDL_Surface * get_surface() const
    {
        return _surface;
    }
};


#endif
