/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Thread wrapper for thread management
*/

#pragma once

#include <thread>
#include <functional>
#include <stdexcept>

namespace Plazza {

/**
 * @class Thread
 * @brief RAII wrapper for std::thread
 * Provides clean encapsulation and automatic cleanup
 */
class Thread
{
    public:
        /**
         * @brief Constructor - does not start thread
         */
        Thread() = default;

        /**
         * @brief Constructor - starts thread with callable
         * @param func Function/callable to run in thread
         * @param args Arguments to pass to func
         */
        template <typename Function, typename... Args>
        explicit Thread(Function &&func, Args &&...args)
        {
            _thread = std::thread(std::forward<Function>(func), std::forward<Args>(args)...);
        }

        /**
         * @brief Destructor - joins thread if joinable
         */
        ~Thread()
        {
            if (_thread.joinable()) {
                _thread.join();
            }
        }

        /* Delete copy operations - threads are not copyable */
        Thread(const Thread &) = delete;
        Thread &operator=(const Thread &) = delete;

        /* Allow move operations */
        Thread(Thread &&other) noexcept : _thread(std::move(other._thread)) {}
        Thread &operator=(Thread &&other) noexcept
        {
            if (this != &other) {
                if (_thread.joinable()) {
                    _thread.join();
                }
                _thread = std::move(other._thread);
            }
            return *this;
        }

        /**
         * @brief Check if thread is joinable
         * @return true if joinable, false otherwise
         */
        bool joinable() const { return _thread.joinable(); }

        /**
         * @brief Join thread (wait for completion)
         * @throws std::runtime_error if thread not joinable
         */
        void join()
        {
            if (!_thread.joinable()) {
                throw std::runtime_error("Cannot join non-joinable thread");
            }
            _thread.join();
        }

        /**
         * @brief Detach thread
         * @throws std::runtime_error if thread not joinable
         */
        void detach()
        {
            if (!_thread.joinable()) {
                throw std::runtime_error("Cannot detach non-joinable thread");
            }
            _thread.detach();
        }

        /**
         * @brief Get thread id
         * @return Thread id
         */
        std::thread::id getId() const { return _thread.get_id(); }

        /**
         * @brief Get underlying std::thread (for advanced usage)
         * @return Reference to the std::thread
         */
        std::thread &getNative() { return _thread; }

    private:
        std::thread _thread;
};

} // namespace Plazza
