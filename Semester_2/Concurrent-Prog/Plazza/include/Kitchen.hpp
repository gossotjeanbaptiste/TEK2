/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Kitchen class definition - manages pizza cooking with thread pool
*/
#pragma once
#include "Pizza.hpp"
#include "Stock.hpp"
#include "Cook.hpp"
#include "Mutex.hpp"
#include "ConditionVariable.hpp"
#include "Thread.hpp"
#include <queue>
#include <vector>
#include <cstdint>

namespace Plazza {

/**
 * @class Kitchen
 * @brief Manages pizza cooking with a pool of cook threads
 * Kitchen maintains a thread-safe queue of pizzas, a local stock,
 * and a thread pool of workers that cook pizzas sequentially.
 * 
 * Capacity is capped at 2*N where N is the number of cooks.
 */
    class Kitchen {
        public:
            /**
             * @brief Constructor - creates kitchen with N cooks
             * @param numCooks Number of cook threads
             * @param cookingMultiplier Cooking time multiplier (e.g., 2.0 = double time)
             * @param refillTimeMs Time in milliseconds between stock refills
             * @throws std::invalid_argument if numCooks is 0
             */
            Kitchen(uint32_t numCooks, float cookingMultiplier, uint32_t refillTimeMs);

            /**
             * @brief Destructor - shuts down kitchen and workers
             */
            ~Kitchen();

            /* Delete copy operations */
            Kitchen(const Kitchen &) = delete;
            Kitchen &operator=(const Kitchen &) = delete;

            /**
             * @brief Add a pizza to the cooking queue
             * @param pizza Pizza to cook
             * @return true if pizza was added, false if queue is full
             * @throws std::runtime_error if kitchen is shut down
             */
            bool addPizza(const Pizza &pizza);

            /**
             * @brief Get current number of pizzas in queue (waiting + cooking)
             * @return Queue size
             */
            uint32_t getQueueSize() const;

            /**
             * @brief Get maximum capacity (2 * numCooks)
             * @return Maximum number of pizzas allowed
             */
            uint32_t getMaxCapacity() const { return _maxCapacity; }

            /**
             * @brief Check if kitchen is full (can't accept more pizzas)
             * @return true if queue size >= capacity
             */
            bool isFull() const;

            /**
             * @brief Get number of cooks (workers)
             * @return Number of cooks
             */
            uint32_t getNumCooks() const { return _numCooks; }

            /**
             * @brief Shutdown the kitchen gracefully
             * Waits for all pizzas to be cooked before returning
             */
            void shutdown();

            /**
             * @brief Check if kitchen is shut down
             * @return true if shut down
             */
            bool isShutdown() const;

            /**
             * @brief Get number of pizzas cooked so far
             * @return Number of cooked pizzas
             */
            uint32_t getPizzasCooked() const;

            /**
             * @brief Get reference to stock for status display
             * @return Reference to stock
             */
            const Stock &getStock() const { return _stock; }

        private:
            std::vector<Thread> _workers;
            Thread _refillWorker;  // Thread for refilling stock
            std::queue<Pizza> _pizzaQueue;
            Stock _stock;
            Cook _cook;
            
            mutable Mutex _queueMutex;
            ConditionVariable _pizzaAvailable;  // Signals when pizza added to queue
            ConditionVariable _queueNotFull;    // Signals when space available
            ConditionVariable _stockRefilled;   // Signals when stock is refilled
            
            uint32_t _numCooks;
            uint32_t _maxCapacity;  // 2 * numCooks
            uint32_t _refillTimeMs;
            bool _shutdown;
            uint32_t _pizzasCooked;  // Statistics

            /**
             * @brief Worker function executed by each cook thread
             * Continuously waits for pizzas and cooks them
             */
            void workerLoop();

            /**
             * @brief Refill thread function that periodically restocks ingredients
             * Runs on a separate thread and refills ingredients every _refillTimeMs
             */
            void refillLoop();
    };
} // namespace Plazza
