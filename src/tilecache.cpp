#include "tilecache.hpp"

#include <iostream>
#include <sstream>

TileCache::TileCache(std::string tile_dir, SDL_Surface * surface)
: _tile_dir(tile_dir), _ref_surface(surface)
{
    std::cout << "Creating TileCache..." << std::endl;
}

TileCache::~TileCache()
{
    for(map_t::iterator i = _cache.begin(); i != _cache.end(); ++i)
    {
        if(i->second != NULL)
        {
            delete i->second;
            i->second = NULL;
        }
    }
}

SDL_Surface * TileCache::get_tile(int level, int i, int j)
{
    if(level < 0 )
        return NULL;
    if(i < 0 || j < 0 || i >= (1<<level) || j >= (1<<level))
        return NULL;

    key_t key = make_key(level, i, j);
    
    map_t::iterator p = _cache.find(key);

    if(p == _cache.end())
    {
        std::cout << key << std::flush;
        
        std::string file_name = make_file_name(level, i, j);
        
        TileCacheItem * item = new TileCacheItem(file_name, _ref_surface);
        
        p = _cache.insert(make_pair(key, item)).first;
        
        std::cout << '\t' << _cache.size() << std::endl;
    }

    return p->second->get_surface();
}

TileCache::key_t TileCache::make_key(int level, int i, int j)
{
    std::stringstream ss;
    ss << level << '/' << i << '/' << j;

    return ss.str();
}

std::string TileCache::make_file_name(int level, int i, int j)
{
    std::stringstream ss;
    ss << _tile_dir;
    ss << level << '/' << i << '/' << j << ".png";

    return ss.str();
}

