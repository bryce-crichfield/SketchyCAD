#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <optional>

namespace Task
{

template <typename T> class BlockingQueue
{
    std::mutex mutex;
    std::condition_variable condition;
    std::queue<T> queue;

  public:
    BlockingQueue() = default;
    ~BlockingQueue() = default;

    void Push(T item)
    {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(item);
        lock.unlock();
        condition.notify_one();
    }

    std::optional<T> Pop() 
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty())
        {
            return std::nullopt;
        }

        T item = queue.front();
        queue.pop();
        return item;
    }

    bool IsEmpty()
    {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }
};

class Context
{
    std::atomic<bool> isRunning;
    std::atomic<unsigned> active_thread_count = 0;

    BlockingQueue<std::function<void()>> queue;
    std::vector<std::thread> worker_threads;

    void Worker()
    {
        while (isRunning)
        {
            auto task = queue.Pop();
            if (task)
            {
                active_thread_count++;
                (*task)();
                active_thread_count--;
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

  public:
    Context() 
    {
        isRunning = true;

        unsigned thread_count = std::thread::hardware_concurrency();
        for (unsigned i = 0; i < thread_count; i++)
        {
            worker_threads.emplace_back(&Context::Worker, this);
        }
    }

    ~Context()
    {
        isRunning = false;

        for (auto &thread : worker_threads)
        {
            thread.join();
        }
    }

    unsigned GetThreadCount() const
    {
        return worker_threads.size();
    }

    template <typename TaskFunction> void Submit(TaskFunction task)
    {
        auto task_wrapper = [task]() { task(); };
        queue.Push(task_wrapper);
    }

    void Wait()
    {
        while (!queue.IsEmpty() || active_thread_count != 0)
        {

            std::this_thread::yield();
        }
    }
};
} // namespace Compute