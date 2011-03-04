#ifndef TILECACHE_HPP_INCLUDED
#define TILECACHE_HPP_INCLUDED

#include <map>
#include <string>

#include <SDL/SDL.h>

#include "tilecacheitem.hpp"

class TileCache
{
    typedef std::string key_t;
    typedef std::map<key_t, TileCacheItem *> map_t;

    std::string _tile_dir;
    map_t _cache;
    SDL_Surface * _ref_surface;
    
    static key_t make_key(int level, int i, int j);
    std::string make_file_name(int level, int i, int j);

public:
    TileCache(std::string tile_dir, SDL_Surface * surface);
    ~TileCache();
    
    SDL_Surface * get_tile(int level, int i, int j);
};

#endif
