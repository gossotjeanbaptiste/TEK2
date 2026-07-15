/*
** EPITECH PROJECT, 2024
** Plazza
** File description:
** Kitchen class implementation
*/

#include "Kitchen.hpp"
#include "Reception.hpp"
#include <stdexcept>
#include <chrono>
#include <thread>
#include <iostream>

using namespace Plazza;

Kitchen::Kitchen(uint32_t numCooks, float cookingMultiplier, uint32_t refillTimeMs)
    : _stock(refillTimeMs, 5),  // 5 initial units of each ingredient
      _cook(cookingMultiplier),
      _numCooks(numCooks),
      _maxCapacity(2 * numCooks),
      _refillTimeMs(refillTimeMs),
      _shutdown(false),
      _pizzasCooked(0)
{
    if (numCooks == 0)
        throw std::invalid_argument("Kitchen must have at least 1 cook");
    // Create worker threads for each cook
    for (uint32_t i = 0; i < numCooks; ++i)
        _workers.emplace_back([this]() { this->workerLoop(); });
    // Create refill thread
    _refillWorker = Thread([this]() { this->refillLoop(); });
}

Kitchen::~Kitchen()
{
    shutdown();
}

void Kitchen::workerLoop()
{
    while (true) {
        Pizza pizza;
        bool gotPizza = false;
        {
            LockGuard lock(_queueMutex);
            // Keep trying to get a pizza we can cook
            while (true) {
                // Check if we should exit
                if (_shutdown && _pizzaQueue.empty())
                    return;  // Exit thread
                // Wait for notification if queue is empty
                if (_pizzaQueue.empty()) {
                    _pizzaAvailable.wait(lock);
                    if (_shutdown && _pizzaQueue.empty())
                        return;  // Exit thread
                }
                // Try to cook the front pizza
                if (!_pizzaQueue.empty()) {
                    pizza = _pizzaQueue.front();
                    auto requiredIngredients = pizza.getRequiredIngredients();
                    // Only check if we have ingredients, don't consume yet
                    if (_stock.hasAllIngredients(requiredIngredients)) {
                        _pizzaQueue.pop();  // Remove from queue
                        _queueNotFull.notifyAll();
                        gotPizza = true;
                        break;  // Got a pizza, exit inner loop
                    }
                    // Ingredients not available - wait for refill
                    _pizzaAvailable.wait(lock);
                } else {
                    // Queue is empty - wait for notification
                    _pizzaAvailable.wait(lock);
                }
            }
        }
        // Cook OUTSIDE the lock (Cook will consume ingredients)
        if (gotPizza) {
            try {
                _cook.cook(pizza, _stock);
                {
                    LockGuard lock(_queueMutex);
                    _pizzasCooked++;
                }
                // Display and log pizza ready
                std::cout << pizza.getTypeName() << " " << pizza.getSizeName() << " ready" << std::endl;
                Logger::log("Pizza ready: " + pizza.getTypeName() + " " + pizza.getSizeName());
            } catch (const std::exception &e) {
                // Should not happen if we checked hasAllIngredients
                // But if it does, pizza is lost (already removed from queue)
            }
        }
    }
}

void Kitchen::refillLoop()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_refillTimeMs));
        {
            LockGuard lock(_queueMutex);
            if (_shutdown)
                break;
            _stock.refillIngredients();
            _pizzaAvailable.notifyAll();
        }
    }
}

bool Kitchen::addPizza(const Pizza &pizza)
{
    LockGuard lock(_queueMutex);
    if (_shutdown)
        throw std::runtime_error("Kitchen is shut down");

    // Check if queue is full
    if (_pizzaQueue.size() >= _maxCapacity)
        return false;  // Kitchen is full
    _pizzaQueue.push(pizza);
    _pizzaAvailable.notifyOne();  // Wake up a worker thread
    return true;
}

uint32_t Kitchen::getQueueSize() const
{
    LockGuard lock(_queueMutex);
    return _pizzaQueue.size();
}

bool Kitchen::isFull() const
{
    LockGuard lock(_queueMutex);
    return _pizzaQueue.size() >= _maxCapacity;
}

void Kitchen::shutdown()
{
    {
        LockGuard lock(_queueMutex);
        if (_shutdown)
            return;  // Already shut down
        _shutdown = true;
    }

    // Wake up all workers so they can exit
    _pizzaAvailable.notifyAll();
    // Wait for all workers to finish
    for (auto &worker : _workers)
        if (worker.joinable())
            worker.join();
    // Wait for refill worker to finish
    if (_refillWorker.joinable())
        _refillWorker.join();
}

bool Kitchen::isShutdown() const
{
    LockGuard lock(_queueMutex);
    return _shutdown;
}

uint32_t Kitchen::getPizzasCooked() const
{
    LockGuard lock(_queueMutex);
    return _pizzasCooked;
}
