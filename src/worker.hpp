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
#include <utility>
#include <vector>

template <typename job_type>
class WorkerPool
{
public:
    explicit WorkerPool(size_t nthreads = 1);
    ~WorkerPool();
    
    template<typename... ArgTypes>
    void emplace(ArgTypes&&... args);
    template<typename ArgType>
    void push(ArgType&& job);
    void stop();

private:
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<job_type> queue_;
    std::vector<std::thread> threads_;
    bool stop_;

    static int worker_thread(WorkerPool<job_type> * pool);
};

template <typename job_type>
WorkerPool<job_type>::WorkerPool(size_t nthreads)
    : stop_(false)
{
    threads_.reserve(nthreads);
    for(size_t i = 0; i < nthreads; ++i)
    {
        threads_.emplace_back(worker_thread, this);
    }
}

template <typename job_type>
WorkerPool<job_type>::~WorkerPool()
{
    stop();

    for(auto&& t: threads_)
    {
        t.join();
    }
}

template <typename job_type>
int WorkerPool<job_type>::worker_thread(WorkerPool<job_type> *pool)
{
    std::unique_lock<std::mutex> lock(pool->mutex_);
    while(!pool->stop_)
    {
        if(pool->queue_.empty())
        {
            pool->cond_.wait(lock);
            continue;
        }
        
        job_type job(std::move(pool->queue_.front()));
        pool->queue_.pop();
        
        lock.unlock();
        
        // Do the work
        job();
        
        lock.lock();
    }
    
    return 0;
}

template <typename job_type> template <typename... ArgTypes>
void WorkerPool<job_type>::emplace(ArgTypes&&... args)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.emplace(std::forward<ArgTypes>(args)...);
    cond_.notify_one();
}

template <typename job_type> template <typename ArgType>
void WorkerPool<job_type>::push(ArgType&& job)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::forward<ArgType>(job));
    cond_.notify_one();
}

template <typename job_type>
void WorkerPool<job_type>::stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    stop_ = true;
    cond_.notify_all();
}

#endif
