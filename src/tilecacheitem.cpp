#include "tilecacheitem.hpp"

TileCacheItem::TileCacheItem(std::string file_name)
: _file_name(file_name), _tile(NULL), _surface(NULL)
{
}
    
TileCacheItem::~TileCacheItem()
{
    if(_surface != NULL)
        SDL_FreeSurface(_surface);
    if(_tile != NULL)
        delete _tile;
}

bool TileCacheItem::fetch(SDL_Surface * surface)
{
    bool success = false;
    _tile = new Tile(_file_name);
    if(_tile->ready())
    {
        SDL_Surface * s = SDL_CreateRGBSurface(0, _tile->width(), _tile->height(), 32,
            surface->format->Rmask, surface->format->Gmask,
            surface->format->Bmask, surface->format->Amask);
        _tile->draw_to_SDL(s);
        _surface = SDL_DisplayFormat(s);
        success = true;
    }
    delete _tile;
    _tile = NULL;
    return success;
}
