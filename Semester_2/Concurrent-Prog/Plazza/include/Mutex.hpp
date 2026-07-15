/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Mutex wrapper for thread synchronization
*/

#pragma once

#include <mutex>
#include <stdexcept>

namespace Plazza {
    /**
     * @class Mutex
     * @brief RAII wrapper for std::mutex
     * Provides clean encapsulation of mutex locking/unlocking
     */
    class Mutex
    {
        public:
            /**
             * @brief Constructor
             */
            Mutex() = default;

            /**
             * @brief Destructor
             */
            ~Mutex() = default;

            /* Delete copy operations - mutexes are not copyable */
            Mutex(const Mutex &) = delete;
            Mutex &operator=(const Mutex &) = delete;

            /**
             * @brief Lock the mutex (blocks until acquired)
             */
            void lock()
            {
                _mutex.lock();
            }

            /**
             * @brief Try to lock the mutex (non-blocking)
             * @return true if locked, false if already locked
             */
            bool tryLock()
            {
                return _mutex.try_lock();
            }

            /**
             * @brief Unlock the mutex
             */
            void unlock()
            {
                _mutex.unlock();
            }

            /**
             * @brief Get underlying std::mutex (for advanced usage)
             * @return Reference to the std::mutex
             */
            std::mutex &getNative() { return _mutex; }

        private:
            std::mutex _mutex;

            friend class ConditionVariable;
    };

    /**
     * @class LockGuard
     * @brief RAII lock guard - automatically locks/unlocks mutex
     * Prevents deadlocks and ensures mutex is always unlocked
     */
    class LockGuard
    {
        public:
            /**
             * @brief Constructor - acquires lock
             * @param mutex The mutex to lock
             */
            explicit LockGuard(Mutex &mutex) : _mutex(mutex)
            {
                _mutex.lock();
            }

            /**
             * @brief Destructor - releases lock
             */
            ~LockGuard()
            {
                _mutex.unlock();
            }

            /* Delete copy operations */
            LockGuard(const LockGuard &) = delete;
            LockGuard &operator=(const LockGuard &) = delete;

        private:
            Mutex &_mutex;

            friend class ConditionVariable;
    };

} // namespace Plazza