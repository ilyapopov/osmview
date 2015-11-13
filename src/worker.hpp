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

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

template <typename Job>
class WorkerPool
{
public:
    explicit WorkerPool(size_t nthreads);
    ~WorkerPool();
    
    template<typename... ArgTypes>
    void emplace(ArgTypes&&... args);

    template<typename ArgType>
    void push(ArgType&& job);

    void stop();

private:
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<Job> queue_;
    std::vector<std::thread> threads_;
    std::atomic<bool> stop_;

    static void worker_func(WorkerPool<Job> * pool);
};

template <typename Job>
WorkerPool<Job>::WorkerPool(size_t nthreads)
    : stop_(false)
{
    threads_.reserve(nthreads);
    for (size_t i = 0; i < nthreads; ++i)
    {
        threads_.emplace_back(worker_func, this);
    }
}

template <typename Job>
WorkerPool<Job>::~WorkerPool()
{
    stop();

    for (auto&& t: threads_)
    {
        t.join();
    }
}

template <typename Job>
void WorkerPool<Job>::worker_func(WorkerPool<Job> *pool)
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(pool->mutex_);
        pool->cond_.wait(lock, [pool](){
            return !pool->queue_.empty() || pool->stop_;
        });

        if (pool->stop_)
            return;

        Job job(std::move(pool->queue_.front()));
        pool->queue_.pop();

        lock.unlock();

        // Do the work
        job();
    }
}

template <typename Job> template <typename... ArgTypes>
void WorkerPool<Job>::emplace(ArgTypes&&... args)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.emplace(std::forward<ArgTypes>(args)...);
    }
    cond_.notify_one();
}

template <typename Job> template <typename U>
void WorkerPool<Job>::push(U&& job)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::forward<U>(job));
    }
    cond_.notify_one();
}

template <typename Job>
void WorkerPool<Job>::stop()
{
    stop_ = true;
    cond_.notify_all();
}

#endif
