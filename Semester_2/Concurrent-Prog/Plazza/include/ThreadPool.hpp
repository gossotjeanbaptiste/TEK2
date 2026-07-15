/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** ThreadPool class definition
*/

#pragma once

#include "Thread.hpp"
#include "Mutex.hpp"
#include "ConditionVariable.hpp"
#include <queue>
#include <functional>
#include <vector>
#include <cstdint>

namespace Plazza {

    /**
     * @class ThreadPool
     * @brief Thread pool for managing a fixed number of worker threads
     * Uses a task queue with thread-safe access via Mutex and ConditionVariable
     */
    class ThreadPool {
        public:
            /**
             * @brief Constructor - creates worker threads
             * @param numWorkers Number of worker threads to spawn
             * @throws std::runtime_error if numWorkers is 0
             * ! using explicit to prevent accidental implicit conversions from integers
             */
            explicit ThreadPool(uint32_t numWorkers);

            /**
             * @brief Destructor - gracefully shuts down worker threads
             */
            ~ThreadPool();

            /* Delete copy operations */
            ThreadPool(const ThreadPool &) = delete;
            ThreadPool &operator=(const ThreadPool &) = delete;

            /**
             * @brief Enqueue a task to be executed by a worker thread
             * @param task A callable (function, lambda, etc.) to execute
             * @throws std::runtime_error if thread pool is shut down
             */
            void enqueue(std::function<void()> task);

            /**
             * @brief Wait for all queued tasks to complete
             * Does NOT wait for new tasks added after this call
             */
            void wait();

            /**
             * @brief Shutdown the thread pool and wait for all workers
             * Can be called multiple times safely
             */
            void shutdown();

            /**
             * @brief Check if thread pool is shut down
             * @return true if shutdown, false otherwise
             */
            bool isShutdown() const;

            /**
             * @brief Get number of worker threads
             * @return Number of workers
             */
            uint32_t getNumWorkers() const { return _numWorkers; }

            /**
             * @brief Get current queue size (for monitoring)
             * @return Number of pending tasks
             */
            uint32_t getQueueSize() const;

        private:
            std::vector<Thread> _workers;
            std::queue<std::function<void()>> _tasks;
            mutable Mutex _mutex;
            ConditionVariable _condition;
            ConditionVariable _workFinished;
            bool _shutdown;
            uint32_t _numWorkers;
            uint32_t _activeTasks;  // Tasks currently being processed

            /**
             * @brief Worker thread function
             * Continuously processes tasks from the queue
             */
            void workerThread();
    };
}
