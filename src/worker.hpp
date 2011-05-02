#ifndef WORKER_HPP_INCLUDED
#define WORKER_HPP_INCLUDED

#include <queue>
#include <thread>
#include <vector>

template <typename job_type>
class WorkerPool
{
    std::mutex _mutex;
    std::condition_variable _new_commands;
    std::queue<job_type> _queue;
    bool _stop;
    std::vector<std::thread *> _threads;

    static int worker_thread(void * param);
    void stop();

public:
    WorkerPool(int nthreads = 1);
    ~WorkerPool();
    
    void enqueue(job_type job);
    void clear();
};

template <typename job_type>
WorkerPool<job_type>::WorkerPool(int nthreads)
: 
    _stop(false)
{
    for(int i = 0; i < nthreads; i++)
    {
        std::thread * thread = new std::thread(worker_thread, this);
        _threads.push_back(thread);
    }
}

template <typename job_type>
WorkerPool<job_type>::~WorkerPool()
{
    stop();
}

template <typename job_type>
int WorkerPool<job_type>::worker_thread(void * param)
{
    WorkerPool * worker = reinterpret_cast<WorkerPool *>(param);
    
    std::unique_lock<std::mutex> lock(worker->_mutex);
    while(true)
    {
        if(worker->_stop)
        {
            break;
        }
        if(worker->_queue.empty())
        {
            worker->_new_commands.wait(lock);
            continue;
        }
        
        job_type job = worker->_queue.front();
        worker->_queue.pop();
        
        lock.unlock();
        
        // Do the work
        job();
        
        lock.lock();
    }
    lock.unlock();
    
    return 0;
}

template <typename job_type>
void WorkerPool<job_type>::enqueue(job_type job)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _queue.push(job);
    _new_commands.notify_one();
}

template <typename job_type>
void WorkerPool<job_type>::stop()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _stop = true;
        _new_commands.notify_all();
    }

    for(std::vector<std::thread *>::iterator i = _threads.begin(); i != _threads.end(); ++i)
    {
        (*i)->join();
        delete *i;
        *i = NULL;
    }
}

template <typename job_type>
void WorkerPool<job_type>::clear()
{
    std::lock_guard<std::mutex> lock(_mutex);
    
    while(!_queue.empty())
        _queue.pop();
}

#endif
