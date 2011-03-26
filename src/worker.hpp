#ifndef WORKER_HPP_INCLUDED
#define WORKER_HPP_INCLUDED

#include <queue>

#include <SDL/SDL.h>

template <typename job_type>
class Worker
{
    static int worker_thread(void * param);
    SDL_mutex * _thread_mutex;
    SDL_cond * _new_commands;
    std::queue<job_type> _queue;
    bool _stop;
    SDL_Thread * _thread;

public:
    Worker();
    
    void enqueue(job_type job);
    void stop();
};

template <typename job_type>
Worker<job_type>::Worker():
_thread_mutex(SDL_CreateMutex()), _new_commands(SDL_CreateCond()),
_stop(false),
_thread(NULL)
{
    _thread = SDL_CreateThread(worker_thread, this);
}

template <typename job_type>
int Worker<job_type>::worker_thread(void * param)
{
    Worker * tile_cache = reinterpret_cast<Worker *>(param);
    
    int fetched = 0;
    
    SDL_LockMutex(tile_cache->_thread_mutex);
    while(true)
    {
        if(tile_cache->_stop)
        {
            break;
        }
        if(tile_cache->_queue.empty())
        {
            SDL_CondWait(tile_cache->_new_commands, tile_cache->_thread_mutex);
            continue;
        }
        
        job_type job = tile_cache->_queue.front();
        tile_cache->_queue.pop();
        
        SDL_UnlockMutex(tile_cache->_thread_mutex);
        
        // Do the work
        job();
        
        SDL_LockMutex(tile_cache->_thread_mutex);
    }
    SDL_UnlockMutex(tile_cache->_thread_mutex);
    
    return fetched;
}

template <typename job_type>
void Worker<job_type>::enqueue(job_type job)
{
    SDL_LockMutex(_thread_mutex);

    _queue.push(job);
    SDL_CondSignal(_new_commands);
    
    SDL_UnlockMutex(_thread_mutex);
}

template <typename job_type>
void Worker<job_type>::stop()
{
    SDL_LockMutex(_thread_mutex);
    _stop = true;
    SDL_CondBroadcast(_new_commands);
    SDL_UnlockMutex(_thread_mutex);

    SDL_WaitThread(_thread, NULL);
}

#endif
