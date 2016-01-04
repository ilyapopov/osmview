/*
    Copyright 2011, 2014, 2015, Ilya Popov <ilia_popov@rambler.ru>

    This file is part of osmview.
    https://bitbucket.org/ipopov/osmview

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

namespace osmview
{

template <typename Task>
class WorkerPool
{
public:
    explicit WorkerPool(size_t nthreads = std::max(1u, std::thread::hardware_concurrency()));
    ~WorkerPool();
    
    template<typename... ArgTypes>
    void emplace(ArgTypes&&... args);

    template<typename ArgType>
    void push(ArgType&& job);

    void stop();

private:
    enum class Command
    {
        none,
        stop,
        finish,
    };

    std::vector<std::thread> threads_;

    std::mutex command_mutex_;
    std::condition_variable command_cond_var_;
    std::queue<Task> task_queue_;
    Command command_;

    void worker_func();
};

template <typename Task>
WorkerPool<Task>::WorkerPool(size_t nthreads)
    : command_(Command::none)
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
            return !task_queue_.empty() || command_ != Command::none;
        });

        if (command_ == Command::stop)
            break;
        if (command_ == Command::finish && task_queue_.empty())
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
        command_ = Command::stop;
    }
    command_cond_var_.notify_all();
}

} // namespace

#endif
