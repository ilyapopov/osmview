#ifndef TILE_FETCHER_HPP_INCLUDED
#define TILE_FETCHER_HPP_INCLUDED

#include <string>
#include <queue>

#include <SDL/SDL.h>

#include "tilecacheitem.hpp"

class TileFetcher
{
    static int tile_fetcher_thread(void * param);
    SDL_mutex * _fetch_thread_mutex;
    SDL_cond * _new_commands;
    std::queue<TileCacheItem *> _fetch_queue;
    bool _fetch_stop;
    SDL_Thread * _fetch_thread;

public:
    TileFetcher();
    
    void enqueue(TileCacheItem * item);
    void stop();        
};

#endif
