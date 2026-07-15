/*
** EPITECH PROJECT, 2026
** PLAZZA
** File description:
** Reception class implementation
*/

#include "Reception.hpp"
#include <iostream>
#include <iomanip>
#include <climits>
#include <thread>
#include <chrono>
#include <fstream>
#include <ctime>

using namespace Plazza;

// Logger implementation
const char *Logger::_logFile = "plazza.log";

void Logger::initialize()
{
    // Delete existing log file
    std::remove(_logFile);
    
    // Write header
    std::ofstream log(_logFile, std::ios::app);
    if (log.is_open()) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        log << "=== PLAZZA LOG - " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " ===" << std::endl;
        log.close();
    }
}

void Logger::log(const std::string &message)
{
    std::ofstream log(_logFile, std::ios::app);
    if (log.is_open()) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        log << "[" << std::put_time(&tm, "%H:%M:%S") << "] " << message << std::endl;
        log.close();
    }
}

Reception::Reception(float cookingMultiplier, int cooksPerKitchen, int restockTimeMs)
    : _cookingMultiplier(cookingMultiplier),
      _cooksPerKitchen(cooksPerKitchen),
      _restockTimeMs(restockTimeMs),
      _nextKitchenId(0)
{
    Logger::initialize();
}

Reception::~Reception()
{
    for (auto &thread : _kitchenThreads)
        if (thread.joinable())
            thread.join();
}

void Reception::displayPrompt() const
{
    std::cout << "[Plazza]> " << std::flush;
}

void Reception::run()
{
    std::string command;
    std::cout << "╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║     PLAZZA - Pizzeria Management       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  <order>  - Order pizzas (e.g., 'regina M x2; margarita S x1')" << std::endl;
    std::cout << "  status   - Display status" << std::endl;
    std::cout << "  quit     - Close program" << std::endl;
    bool running = true;
    while (running) {
        displayPrompt();
        if (!std::getline(std::cin, command))
            break;
        if (command.empty())
            continue;
        
        try {
            running = handleCommand(command);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    shutdownAllKitchens();
    std::cout << "Goodbye!" << std::endl;
}

bool Reception::handleCommand(const std::string &command)
{
    if (CommandParser::isExitCommand(command))
        return false;
    if (CommandParser::isStatusCommand(command)) {
        displayStatus();
        return true;
    }
    try {
        processOrder(command);
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Invalid order: " << e.what() << std::endl;
        return true;
    }
}

void Reception::processOrder(const std::string &orderString)
{
    std::vector<Pizza> pizzas = _parser.parseOrder(orderString);
    
    if (pizzas.empty())
        throw std::invalid_argument("No valid pizzas");
    
    std::cout << "Order received: " << pizzas.size() << " pizza(s)" << std::endl;
    for (const auto &pizza : pizzas) {
        std::cout << "  - " << pizza.getTypeName() << " " << pizza.getSizeName() << std::endl;
        Logger::log("Pizza ordered: " + pizza.getTypeName() + " " + pizza.getSizeName());
    }
    dispatchPizzas(pizzas);
}

void Reception::displayStatus() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex &>(_kitchensMutex));
    std::cout << "\nStatus: " << _kitchens.size() << " kitchen(s) active" << std::endl;
    std::cout << "  Multiplier: " << _cookingMultiplier << "x" << std::endl;
    std::cout << "  Cooks per kitchen: " << _cooksPerKitchen << std::endl;
    std::cout << "  Restock time: " << _restockTimeMs << "ms" << std::endl;
    uint32_t totalCooked = 0;
    uint32_t totalWaiting = 0;
    
    for (size_t i = 0; i < _kitchens.size(); ++i) {
        if (_kitchens[i]) {
            uint32_t cooked = _kitchens[i]->getPizzasCooked();
            uint32_t waiting = _kitchens[i]->getQueueSize();
            totalCooked += cooked;
            totalWaiting += waiting;
            std::cout << "  Kitchen " << i << ": "
                      << cooked << " cooked, "
                      << waiting << " / "
                      << _kitchens[i]->getMaxCapacity() << " in queue" << std::endl;
            // Display stock for this kitchen
            const auto &stock = _kitchens[i]->getStock();
            const auto &ingredients = stock.getAllIngredients();
            std::cout << "    Stock: ";
            bool first = true;
            for (const auto &pair : ingredients) {
                if (!first) std::cout << ", ";
                std::cout << pair.second.getTypeName() << "(" << stock.getQuantity(pair.first) << ")";
                first = false;
            }
            std::cout << std::endl;
        }
    }
    std::cout << "  Total: " << totalCooked << " pizzas cooked, "
              << totalWaiting << " waiting" << std::endl;
}

void Reception::dispatchPizzas(const std::vector<Pizza> &pizzas)
{
    for (const auto &pizza : pizzas) {
        bool added = false;
        while (!added) {
            int kitchenId = findBestKitchen();
            {
                std::lock_guard<std::mutex> lock(_kitchensMutex);
                if (kitchenId >= 0 && kitchenId < static_cast<int>(_kitchens.size()))
                    if (_kitchens[kitchenId])
                        if (_kitchens[kitchenId]->addPizza(pizza)) {
                            std::cout << "  Dispatched to kitchen " << kitchenId << std::endl;
                            Logger::log("Pizza " + pizza.getTypeName() + " " + pizza.getSizeName() + 
                                      " dispatched to kitchen " + std::to_string(kitchenId));
                            added = true;
                        }
            }
            if (!added)
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

int Reception::findBestKitchen()
{
    std::lock_guard<std::mutex> lock(_kitchensMutex);
    
    if (_kitchens.empty()) {
        return createKitchen();
    }
    
    // Find kitchen with lowest queue
    int bestId = 0;
    int minQueue = INT_MAX;
    int numCooks = _cooksPerKitchen;
    bool anyWithCapacity = false;
    
    for (size_t i = 0; i < _kitchens.size(); ++i) {
        if (_kitchens[i] && !_kitchens[i]->isShutdown()) {
            int queueSize = _kitchens[i]->getQueueSize();
            
            // If queue is low relative to cooks, this kitchen can handle more
            if (queueSize < numCooks) {
                anyWithCapacity = true;
                if (queueSize < minQueue) {
                    minQueue = queueSize;
                    bestId = i;
                }
            }
            else if (queueSize < minQueue) {
                minQueue = queueSize;
                bestId = i;
            }
        }
    }
    // If all kitchens are overloaded (queue >= numCooks), create new one
    if (!anyWithCapacity && minQueue >= numCooks) {
        return createKitchen();
    }
    return bestId;
}

int Reception::createKitchen()
{
    int id = _nextKitchenId++;
    try {
        auto kitchen = std::make_unique<Kitchen>(_cooksPerKitchen, _cookingMultiplier, _restockTimeMs);
        
        std::cout << "Kitchen " << id << " created (cooks: " << _cooksPerKitchen << ")" << std::endl;
        Logger::log("Kitchen " + std::to_string(id) + " created with " + std::to_string(_cooksPerKitchen) + " cooks");
        
        _kitchens.push_back(std::move(kitchen));
        
        // Start a monitoring thread for this kitchen
        _kitchenThreads.emplace_back([this, id]() {
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::lock_guard<std::mutex> lock(_kitchensMutex);
                if (id < static_cast<int>(_kitchens.size()) && _kitchens[id]) {
                    if (_kitchens[id]->isShutdown())
                        break;
                } else {
                    // Si la cuisine a disparu ou l'index n'est plus valide, on arrête le monitoring
                    break;
                }
            }
        std::cout << "Kitchen " << id << " processing complete" << std::endl;
        });
    // Détacher le thread de monitoring pour ne pas bloquer la fermeture
    _kitchenThreads.back().detach();
    } catch (const std::exception &e) {
        std::cerr << "Failed to create kitchen: " << e.what() << std::endl;
        return -1;
    }
    return id;
}

void Reception::shutdownAllKitchens()
{
    // Demander l'arrêt à chaque kitchen (si Kitchen expose shutdown())
    {
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        for (auto &k : _kitchens)
            if (k)
                k->shutdown();
    }
    // Attendre les threads de monitoring
    for (auto &t : _kitchenThreads)
        if (t.joinable())
            t.join();
    _kitchenThreads.clear();
}
