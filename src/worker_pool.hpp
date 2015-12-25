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

#ifndef WORKER_POOL_HPP_INCLUDED
#define WORKER_POOL_HPP_INCLUDED

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

template <typename Task>
class WorkerPool
{
public:
    explicit WorkerPool(size_t nthreads = std::thread::hardware_concurrency());
    ~WorkerPool();
    
    template<typename... ArgTypes>
    void emplace(ArgTypes&&... args);

    template<typename ArgType>
    void push(ArgType&& job);

    void stop();

private:
    enum class command
    {
        none,
        stop,
        finish,
    };

    std::vector<std::thread> threads_;

    std::mutex command_mutex_;
    std::condition_variable command_cond_var_;
    std::queue<Task> task_queue_;
    command command_;

    void worker_func();
};

template <typename Task>
WorkerPool<Task>::WorkerPool(size_t nthreads)
    : command_(command::none)
{
    threads_.reserve(nthreads);
    for (size_t i = 0; i < nthreads; ++i)
    {
        threads_.emplace_back(&WorkerPool::worker_func, this);
    }
}

template <typename Task>
WorkerPool<Task>::~WorkerPool()
{
    stop();

    for (auto&& t: threads_)
    {
        t.join();
    }
}

template <typename Task>
void WorkerPool<Task>::worker_func()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(command_mutex_);
        command_cond_var_.wait(lock, [&](){
            return !task_queue_.empty() || command_ != command::none;
        });

        if (command_ == command::stop)
            break;
        if (command_ == command::finish && task_queue_.empty())
            break;

        Task task(std::move(task_queue_.front()));
        task_queue_.pop();

        lock.unlock();

        // Do the work
        task();

    }
}

template <typename Task> template <typename... ArgTypes>
void WorkerPool<Task>::emplace(ArgTypes&&... args)
{
    {
        std::lock_guard<std::mutex> lock(command_mutex_);
        task_queue_.emplace(std::forward<ArgTypes>(args)...);
    }
    command_cond_var_.notify_one();
}

template <typename Task> template <typename ArgType>
void WorkerPool<Task>::push(ArgType&& task)
{
    {
        std::lock_guard<std::mutex> lock(command_mutex_);
        task_queue_.push(std::forward<ArgType>(task));
    }
    command_cond_var_.notify_one();
}

template <typename Task>
void WorkerPool<Task>::stop()
{
    {
        std::lock_guard<std::mutex> lock(command_mutex_);
        command_ = command::stop;
    }
    command_cond_var_.notify_all();
}

#endif
