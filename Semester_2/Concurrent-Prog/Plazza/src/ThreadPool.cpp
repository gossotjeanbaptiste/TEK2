/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** ThreadPool class implementation
*/

#include "ThreadPool.hpp"
#include <stdexcept>

using namespace Plazza;

ThreadPool::ThreadPool(uint32_t numWorkers)
    : _shutdown(false), _numWorkers(numWorkers), _activeTasks(0)
{
    if (numWorkers == 0)
        throw std::runtime_error("ThreadPool must have at least 1 worker");
    // Create worker threads
    for (uint32_t i = 0; i < numWorkers; ++i)
        _workers.emplace_back([this]() { this->workerThread(); });
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

void ThreadPool::workerThread()
{
    while (true) {
        std::function<void()> task;
        {
            LockGuard lock(_mutex);
            // Wait for task or shutdown signal
            while (_tasks.empty() && !_shutdown)
                _condition.wait(lock);
            // Exit if shutdown and no more tasks
            if (_shutdown && _tasks.empty())
                break;
            // Get task from queue
            if (!_tasks.empty()) {
                task = _tasks.front();
                _tasks.pop();
                _activeTasks++;
            }
        }
        // Execute task outside of lock
        if (task) {
            try {
                task();
            } catch (const std::exception &e) {
                // Catch exceptions to prevent worker thread from crashing
                // In production, you might want to log these
            }
            {
                LockGuard lock(_mutex);
                _activeTasks--;
                _workFinished.notifyAll();
            }
        }
    }
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        LockGuard lock(_mutex);
        if (_shutdown)
            throw std::runtime_error("Cannot enqueue task: ThreadPool is shut down");
        _tasks.push(task);
    }
    _condition.notifyOne();
}

void ThreadPool::wait()
{
    LockGuard lock(_mutex);
    // Wait until queue is empty AND no tasks are being processed
    while (!_tasks.empty() || _activeTasks > 0)
        _workFinished.wait(lock);
}

void ThreadPool::shutdown()
{
    {
        LockGuard lock(_mutex);
        if (_shutdown)
            return;  // Already shut down
        _shutdown = true;
    }
    // Wake up all workers so they can exit
    _condition.notifyAll();
    // Wait for all workers to finish
    for (auto &worker : _workers)
        if (worker.joinable())
            worker.join();
    _workers.clear();
}

bool ThreadPool::isShutdown() const
{
    LockGuard lock(_mutex);
    return _shutdown;
}

uint32_t ThreadPool::getQueueSize() const
{
    LockGuard lock(_mutex);
    return _tasks.size();
}
