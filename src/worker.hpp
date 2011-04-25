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
    ~Worker();
    
    void enqueue(job_type job);
    void stop();
};

template <typename job_type>
Worker<job_type>::Worker()
: _thread_mutex(SDL_CreateMutex()), _new_commands(SDL_CreateCond()),
_stop(false),
_thread(NULL)
{
    _thread = SDL_CreateThread(worker_thread, this);
}

template <typename job_type>
Worker<job_type>::~Worker()
{
    stop();

    if(_thread_mutex)
        SDL_DestroyMutex(_thread_mutex);
    if(_new_commands)
        SDL_DestroyCond(_new_commands);
}

template <typename job_type>
int Worker<job_type>::worker_thread(void * param)
{
    Worker * worker = reinterpret_cast<Worker *>(param);
    
    SDL_LockMutex(worker->_thread_mutex);
    while(true)
    {
        if(worker->_stop)
        {
            break;
        }
        if(worker->_queue.empty())
        {
            SDL_CondWait(worker->_new_commands, worker->_thread_mutex);
            continue;
        }
        
        job_type job = worker->_queue.front();
        worker->_queue.pop();
        
        SDL_UnlockMutex(worker->_thread_mutex);
        
        // Do the work
        job();
        
        SDL_LockMutex(worker->_thread_mutex);
    }
    SDL_UnlockMutex(worker->_thread_mutex);
    
    return 0;
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
