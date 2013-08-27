/*
    Copyright 2011, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.

    osmview is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    osmview is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with osmview.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WORKER_HPP_INCLUDED
#define WORKER_HPP_INCLUDED

#include <condition_variable>
#include <mutex>
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
    std::vector<std::thread> _threads;

    static int worker_thread(void * param);
    void stop();

public:
    WorkerPool(int nthreads = 1);
    WorkerPool(const WorkerPool &) = delete;
    void operator =(const WorkerPool &) = delete;
    ~WorkerPool();
    
    void push(const job_type & job);
    void clear();
};

template <typename job_type>
WorkerPool<job_type>::WorkerPool(int nthreads)
: 
    _stop(false)
{
    for(int i = 0; i < nthreads; i++)
    {
        _threads.push_back(std::thread(worker_thread, this));
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
    while(!worker->_stop)
    {
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
    
    return 0;
}

template <typename job_type>
void WorkerPool<job_type>::push(const job_type &job)
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

    for(auto& t: _threads)
    {
        t.join();
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
