#include "tilecache.hpp"

#include <iostream>
#include <sstream>

TileCache::TileCache(std::string tile_dir)
: _tile_dir(tile_dir),
_fetch_thread_mutex(SDL_CreateMutex()), _new_commands(SDL_CreateCond()),
_fetch_stop(false),
_fetch_thread(NULL)
{
    _fetch_thread = SDL_CreateThread(tile_fetcher_thread, this);
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
    
    SDL_LockMutex(_fetch_thread_mutex);
    _fetch_stop = true;
    SDL_CondBroadcast(_new_commands);
    SDL_UnlockMutex(_fetch_thread_mutex);

    SDL_WaitThread(_fetch_thread, NULL);
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
        std::cout << "Miss: " << key << std::flush;
        
        std::string file_name = make_file_name(level, i, j);
        
        TileCacheItem * item = new TileCacheItem(file_name);
        
        p = _cache.insert(make_pair(key, item)).first;
        
        SDL_LockMutex(_fetch_thread_mutex);
        
        _fetch_queue.push(p->second);
        SDL_CondSignal(_new_commands);
        
        SDL_UnlockMutex(_fetch_thread_mutex);
        
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

int TileCache::tile_fetcher_thread(void * param)
{
    TileCache * tile_cache = reinterpret_cast<TileCache *>(param);
    
    int fetched = 0;
    
    SDL_LockMutex(tile_cache->_fetch_thread_mutex);
    while(true)
    {
        if(tile_cache->_fetch_stop)
        {
            break;
        }
        if(tile_cache->_fetch_queue.empty())
        {
            SDL_CondWait(tile_cache->_new_commands, tile_cache->_fetch_thread_mutex);
            continue;
        }
        
        TileCacheItem * tile_to_fetch = tile_cache->_fetch_queue.front();
        tile_cache->_fetch_queue.pop();
        
        SDL_UnlockMutex(tile_cache->_fetch_thread_mutex);
        
        // Do the work
        tile_to_fetch->fetch();
        fetched += 1;
        
        SDL_LockMutex(tile_cache->_fetch_thread_mutex);
    }
    SDL_UnlockMutex(tile_cache->_fetch_thread_mutex);
    
    return fetched;
}

