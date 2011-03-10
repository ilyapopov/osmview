#ifndef TILECACHE_HPP_INCLUDED
#define TILECACHE_HPP_INCLUDED

#include <queue>
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
    
    static key_t make_key(int level, int i, int j);
    std::string make_file_name(int level, int i, int j);
    
    static int tile_fetcher_thread(void * param);
    SDL_mutex * _fetch_thread_mutex;
    SDL_cond * _new_commands;
    std::queue<TileCacheItem *> _fetch_queue;
    bool _fetch_stop;
    SDL_Thread * _fetch_thread;

public:
    TileCache(std::string tile_dir);
    ~TileCache();
    
    SDL_Surface * get_tile(int level, int i, int j);
};

#endif
