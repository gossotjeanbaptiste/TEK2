/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Process wrapper for process management
*/

#pragma once

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdexcept>
#include <string>

namespace Plazza {

/**
 * @class Process
 * @brief RAII wrapper for process management (fork/exit/wait)
 * Provides clean encapsulation of process lifecycle
 */
class Process
{
    public:
        /**
         * @brief Constructor - creates a new process
         * @param function Callable to execute in child process
         * Can be a std::function, lambda, or function pointer
         */
        template <typename Function>
        explicit Process(Function &&function)
            : _pid(-1), _isChild(false), _exited(false), _exitCode(0)
        {
            _pid = fork();

            if (_pid < 0) {
                throw std::runtime_error("Failed to fork process");
            } else if (_pid == 0) {
                // Child process
                _isChild = true;
                try {
                    function();
                } catch (const std::exception &e) {
                    std::cerr << "Exception in child process: " << e.what() << std::endl;
                    exit(1);
                }
                exit(0);
            }
            // Parent process continues here
        }

        /**
         * @brief Destructor - waits for child process if not already waited
         */
        ~Process()
        {
            if (!_isChild && !_exited && _pid > 0) {
                wait();
            }
        }

        /* Delete copy operations - processes are not copyable */
        Process(const Process &) = delete;
        Process &operator=(const Process &) = delete;

        /* Allow move operations */
        Process(Process &&other) noexcept
            : _pid(other._pid), _isChild(other._isChild), _exited(other._exited),
            _exitCode(other._exitCode)
        {
            other._pid = -1;
            other._isChild = false;
        }

        Process &operator=(Process &&other) noexcept
        {
            if (this != &other) {
                if (!_isChild && !_exited && _pid > 0) {
                    wait();
                }
                _pid = other._pid;
                _isChild = other._isChild;
                _exited = other._exited;
                _exitCode = other._exitCode;
                other._pid = -1;
                other._isChild = false;
            }
            return *this;
        }

        /**
         * @brief Wait for child process to exit
         * @return Exit code of child process
         * @throws std::runtime_error if not in parent process or already waited
         */
        int wait()
        {
            if (_isChild) {
                throw std::runtime_error("Cannot wait in child process");
            }
            if (_exited) {
                throw std::runtime_error("Process already waited");
            }
            if (_pid <= 0) {
                throw std::runtime_error("Invalid process ID");
            }

            int status = 0;
            if (waitpid(_pid, &status, 0) < 0) {
                throw std::runtime_error("waitpid failed");
            }

            _exited = true;
            if (WIFEXITED(status)) {
                _exitCode = WEXITSTATUS(status);
            } else {
                _exitCode = -1; // Abnormal termination
            }

            return _exitCode;
        }

        /**
         * @brief Check if this is child process
         * @return true if child, false if parent
         */
        bool isChild() const { return _isChild; }

        /**
         * @brief Check if process has exited
         * @return true if exited, false otherwise
         */
        bool hasExited() const { return _exited; }

        /**
         * @brief Get exit code
         * @return Exit code (valid only after wait() or if child process)
         */
        int getExitCode() const { return _exitCode; }

        /**
         * @brief Get process ID
         * @return Process ID
         */
        pid_t getPid() const { return _pid; }

        /**
         * @brief Send signal to process
         * @param signal Signal number
         * @return 0 on success, -1 on error
         */
        int sendSignal(int signal) const
        {
            if (_isChild || _pid <= 0) {
                return -1;
            }
            return kill(_pid, signal);
        }

    private:
        pid_t _pid;      // Process ID
        bool _isChild;   // Are we in child process?
        bool _exited;    // Has wait() been called?
        int _exitCode;   // Exit code of child process
};

} // namespace Plazza
