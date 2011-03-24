#include "tilefetcher.hpp"

TileFetcher::TileFetcher():
_fetch_thread_mutex(SDL_CreateMutex()), _new_commands(SDL_CreateCond()),
_fetch_stop(false),
_fetch_thread(NULL)
{
    _fetch_thread = SDL_CreateThread(tile_fetcher_thread, this);
}

int TileFetcher::tile_fetcher_thread(void * param)
{
    TileFetcher * tile_cache = reinterpret_cast<TileFetcher *>(param);
    
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

void TileFetcher::enqueue(TileCacheItem * item)
{
        SDL_LockMutex(_fetch_thread_mutex);
        
        _fetch_queue.push(item);
        SDL_CondSignal(_new_commands);
        
        SDL_UnlockMutex(_fetch_thread_mutex);
}

void TileFetcher::stop()
{
    SDL_LockMutex(_fetch_thread_mutex);
    _fetch_stop = true;
    SDL_CondBroadcast(_new_commands);
    SDL_UnlockMutex(_fetch_thread_mutex);

    SDL_WaitThread(_fetch_thread, NULL);
}
