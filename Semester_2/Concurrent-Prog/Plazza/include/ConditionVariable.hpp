/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** ConditionVariable wrapper for thread synchronization
*/

#pragma once

#include "Mutex.hpp"
#include <condition_variable>
#include <chrono>

namespace Plazza {

/**
 * @class ConditionVariable
 * @brief RAII wrapper for std::condition_variable
 * Provides clean encapsulation for thread notification
 */
class ConditionVariable
{
public:
    /**
     * @brief Constructor
     */
    ConditionVariable() = default;

    /**
     * @brief Destructor
     */
    ~ConditionVariable() = default;

    /* Delete copy operations */
    ConditionVariable(const ConditionVariable &) = delete;
    ConditionVariable &operator=(const ConditionVariable &) = delete;

    /**
     * @brief Wait for condition to be signaled
     * @param lock Locked LockGuard that will be released during wait
     */
    void wait(LockGuard &lock)
    {
        std::unique_lock<std::mutex> ulock(lock._mutex._mutex, std::adopt_lock);
        _condVar.wait(ulock);
        ulock.release(); // Release ownership so LockGuard destructor doesn't unlock twice
    }

    /**
     * @brief Wait with timeout
     * @param lock Locked LockGuard that will be released during wait
     * @param milliseconds Maximum time to wait
     * @return true if signaled, false if timeout
     */
    bool waitFor(LockGuard &lock, uint32_t milliseconds)
    {
        std::unique_lock<std::mutex> ulock(lock._mutex._mutex, std::adopt_lock);
        bool result = _condVar.wait_for(ulock, std::chrono::milliseconds(milliseconds)) ==
                      std::cv_status::no_timeout;
        ulock.release();
        return result;
    }

    /**
     * @brief Notify one waiting thread
     */
    void notifyOne()
    {
        _condVar.notify_one();
    }

    /**
     * @brief Notify all waiting threads
     */
    void notifyAll()
    {
        _condVar.notify_all();
    }

    /**
     * @brief Get underlying std::condition_variable (for advanced usage)
     * @return Reference to the std::condition_variable
     */
    std::condition_variable &getNative() { return _condVar; }

private:
    std::condition_variable _condVar;
};

} // namespace Plazza
