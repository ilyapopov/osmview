#include "tilecacheitem.hpp"

TileCacheItem::TileCacheItem(std::string file_name, SDL_Surface * surface)
: _file_name(file_name), _tile(NULL), _surface(NULL)
{
    _tile = new Tile(_file_name);
    if(_tile->ready())
    {
        _surface = SDL_CreateRGBSurface(0, _tile->width(), _tile->height(), 32,
            surface->format->Rmask, surface->format->Gmask,
            surface->format->Bmask, surface->format->Amask);
        _tile->draw_to_SDL(_surface);
    }
    delete _tile;
    _tile = NULL;
}
    
TileCacheItem::~TileCacheItem()
{
    if(_surface != NULL)
        SDL_FreeSurface(_surface);
    if(_tile != NULL)
        delete _tile;
}

