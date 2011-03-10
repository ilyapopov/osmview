#include "tilecacheitem.hpp"

#include <iostream>

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
    //_surface = s;
    //std::cout << _file_name << " loaded" << std::endl;
    return true;
}
