/*
** EPITECH PROJECT, 2026
** PLAZZA
** File description:
** Main entry point - Interactive pizzeria management system
*/

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "Reception.hpp"

using namespace Plazza;

int main(int argc, char *argv[])
{
    // Validate command line arguments
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <cooking_multiplier> <cooks_per_kitchen> <restock_time_ms>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 2 5 2000" << std::endl;
        return 84;
    }

    try
    {
        // Parse arguments
        float cookingMultiplier = std::stof(argv[1]);
        int cooksPerKitchen = std::stoi(argv[2]);
        int restockTimeMs = std::stoi(argv[3]);

        // Validate arguments
        if (cookingMultiplier <= 0)
        {
            std::cerr << "Error: cooking multiplier must be positive" << std::endl;
            return 84;
        }
        if (cooksPerKitchen <= 0)
        {
            std::cerr << "Error: cooks per kitchen must be positive" << std::endl;
            return 84;
        }
        if (restockTimeMs < 0)
        {
            std::cerr << "Error: restock time must be non-negative" << std::endl;
            return 84;
        }
        Reception reception(cookingMultiplier, cooksPerKitchen, restockTimeMs);
        reception.run();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}

/*
 DEPRECATED: All test functions below
void testStock()
{
    std::cout << "\n=== Testing Stock ===" << std::endl;
    
    // Test 1: Create stock
    Stock stock(1000, 5);  // 1s refill, 5 initial units
    std::cout << "✓ Created stock with initial quantities" << std::endl;
    
    // Test 2: Check ingredients
    uint32_t doughQty = stock.getQuantity(IngredientType::DOUGH);
    std::cout << "✓ Dough quantity: " << doughQty << std::endl;
    
    // Test 3: Consume ingredients
    std::map<IngredientType, uint32_t> ingredients = {
        {IngredientType::DOUGH, 1},
        {IngredientType::TOMATO, 1},
        {IngredientType::GRUYERE, 1}
    };
    
    if (stock.hasAllIngredients(ingredients)) {
        if (stock.consumeIngredients(ingredients)) {
            std::cout << "✓ Successfully consumed margarita ingredients" << std::endl;
        }
    } else {
        std::cout << "✗ Not enough ingredients" << std::endl;
    }
    
    // Test 4: Refill
    stock.refillIngredients();
    std::cout << "✓ Refilled ingredients" << std::endl;
}

void testThreading()
{
    std::cout << "\n=== Testing Threading Abstractions ===" << std::endl;
    
    // Test 1: Mutex and LockGuard
    try {
        Mutex mutex;
        {
            LockGuard lock(mutex);
            std::cout << "✓ Mutex locked/unlocked successfully" << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "✗ Mutex error: " << e.what() << std::endl;
    }
    
    // Test 2: Thread
    try {
        int value = 0;
        Thread t([&value]() {
            value = 42;
        });
        std::cout << "✓ Thread created and executed, value = " << value << std::endl;
    } catch (const std::exception &e) {
        std::cout << "✗ Thread error: " << e.what() << std::endl;
    }
    
    // Test 3: ConditionVariable
    try {
        ConditionVariable condVar;
        std::cout << "✓ ConditionVariable created successfully" << std::endl;
    } catch (const std::exception &e) {
        std::cout << "✗ ConditionVariable error: " << e.what() << std::endl;
    }
}

void testProcess()
{
    std::cout << "\n=== Testing Process ===" << std::endl;
    
    // Test: Create child process
    try {
        Process p([]() {
            std::cout << "✓ Child process executed" << std::endl;
        });
        
        if (!p.isChild()) {
            int code = p.wait();
            std::cout << "✓ Parent process waited for child (exit code: " << code << ")" << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "✗ Process error: " << e.what() << std::endl;
    }
}

void testThreadPool()
{
    std::cout << "\n=== Testing ThreadPool ===" << std::endl;
    
    try {
        // Calculate number of workers: max available - 3
        uint32_t maxThreads = std::thread::hardware_concurrency();
        uint32_t numWorkers = (maxThreads > 3) ? (maxThreads - 3) : 1;
        
        // Test 1: Create thread pool with (max - 3) workers
        ThreadPool pool(numWorkers);
        std::cout << "✓ System has " << maxThreads << " threads available" << std::endl;
        std::cout << "✓ Created ThreadPool with " << pool.getNumWorkers() << " workers (max - 3)" << std::endl;
        
        // Test 2: Enqueue tasks
        int taskCount = numWorkers * 5;  // 5 tasks per worker
        int counter = 0;
        Mutex counterMutex;
        
        for (int i = 0; i < taskCount; ++i) {
            pool.enqueue([&counter, &counterMutex, i]() {
                LockGuard lock(counterMutex);
                counter++;
                std::cout << "  Task " << i << " completed (counter: " << counter << ")" << std::endl;
            });
        }
        
        std::cout << "✓ Enqueued " << taskCount << " tasks" << std::endl;
        
        // Test 3: Wait for all tasks
        pool.wait();
        std::cout << "✓ All tasks completed (final counter: " << counter << ")" << std::endl;
        
        // Test 4: Shutdown
        pool.shutdown();
        std::cout << "✓ ThreadPool shut down gracefully" << std::endl;
        
    } catch (const std::exception &e) {
        std::cout << "✗ ThreadPool error: " << e.what() << std::endl;
    }
}

void testCook()
{
    std::cout << "\n=== Testing Cook ===" << std::endl;
    
    try {
        // Test 1: Create a cook with multiplier (0.1 for faster testing)
        Cook cook(0.1f);
        std::cout << "✓ Created cook with multiplier: " << cook.getCookingMultiplier() << std::endl;
        
        // Test 2: Create a pizza to cook
        Pizza pizza = PizzaBuilder()
            .loadRecipe(PizzaType::MARGARITA)
            .setSize(PizzaSize::M)
            .setId(1)
            .build();
        std::cout << "✓ Created pizza: " << pizza << std::endl;
        
        // Test 3: Create stock with ingredients
        Stock stock(1000, 10);  // 1s refill, 10 initial units
        std::cout << "✓ Created stock with 10 units of each ingredient" << std::endl;
        
        // Test 4: Cook the pizza
        auto startTime = std::chrono::high_resolution_clock::now();
        Pizza cookedPizza = cook.cook(pizza, stock);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "✓ Cooked pizza: " << cookedPizza << std::endl;
        std::cout << "✓ Cooking time: " << duration.count() << "ms" << std::endl;
        
        // Test 5: Try to cook without enough ingredients (should fail)
        try {
            Pizza pizza2 = PizzaBuilder()
                .loadRecipe(PizzaType::REGINA)
                .setSize(PizzaSize::L)
                .setId(2)
                .build();
            
            // Consume ingredients until stock is nearly empty (more aggressively)
            int consumeCount = 0;
            while (consumeCount < 50) {  // Try to consume way more than available
                auto ingredients = pizza2.getRequiredIngredients();
                if (!stock.consumeIngredients(ingredients)) {
                    break;  // Stop when we can't consume anymore
                }
                consumeCount++;
            }
            
            // Now try to cook - should fail
            cook.cook(pizza2, stock);
            std::cout << "✗ Should have failed - not enough ingredients after " << consumeCount << " consumptions" << std::endl;
        } catch (const std::exception &e) {
            std::cout << "✓ Correctly rejected pizza with insufficient ingredients" << std::endl;
        }
        
    } catch (const std::exception &e) {
        std::cout << "✗ Cook error: " << e.what() << std::endl;
    }
}

void testKitchen()
{
    std::cout << "\n=== Testing Kitchen ===" << std::endl;
    
    try {
        // Test 1: Create kitchen with 3 cooks (0.01x = fast for testing)
        Kitchen kitchen(3, 0.01f, 1000);  // 3 cooks, 0.01x multiplier, 1s refill
        std::cout << "✓ Created kitchen with " << kitchen.getNumCooks() << " cooks" << std::endl;
        std::cout << "✓ Max capacity: " << kitchen.getMaxCapacity() << " pizzas (2 * numCooks)" << std::endl;
        
        // Test 2: Add pizzas and watch queue
        std::cout << "\n--- Adding pizzas to queue ---" << std::endl;
        std::vector<Pizza> pizzas;
        for (int i = 0; i < 5; ++i) {
            Pizza p = PizzaBuilder()
                .loadRecipe(static_cast<PizzaType>(1 << (i % 4)))
                .setSize(PizzaSize::M)
                .setId(i + 1)
                .build();
            pizzas.push_back(p);
            
            if (kitchen.addPizza(p)) {
                std::cout << "  Added pizza #" << (i + 1) << " (" << p << ")" << std::endl;
            }
        }
        std::cout << "✓ Queue size after adding: " << kitchen.getQueueSize() << std::endl;
        
        // Test 3: Verify capacity enforcement
        std::cout << "\n--- Testing capacity limit ---" << std::endl;
        int rejected = 0;
        for (int i = 0; i < 5; ++i) {
            Pizza p = PizzaBuilder()
                .loadRecipe(PizzaType::MARGARITA)
                .setSize(PizzaSize::M)
                .setId(100 + i)
                .build();
            
            if (!kitchen.addPizza(p)) {
                rejected++;
            }
        }
        std::cout << "✓ Rejected " << rejected << " pizzas (queue full)" << std::endl;
        
        // Test 4: Wait and monitor processing
        std::cout << "\n--- Monitoring pizza processing ---" << std::endl;
        uint32_t prevSize = kitchen.getQueueSize();
        int checkCount = 0;
        while (kitchen.getQueueSize() > 0 && checkCount < 20) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            uint32_t currentSize = kitchen.getQueueSize();
            if (currentSize != prevSize) {
                std::cout << "  Queue size: " << currentSize << " (processed pizzas)" << std::endl;
                prevSize = currentSize;
            }
            checkCount++;
        }
        
        if (kitchen.getQueueSize() == 0) {
            std::cout << "✓ All pizzas processed successfully!" << std::endl;
        } else {
            std::cout << "⚠ Some pizzas still waiting" << std::endl;
        }
        
        // Test 5: Shutdown and verify
        std::cout << "\n--- Shutting down kitchen ---" << std::endl;
        kitchen.shutdown();
        std::cout << "✓ Kitchen shut down gracefully" << std::endl;
        
        // Verify shutdown
        try {
            Pizza p = PizzaBuilder()
                .loadRecipe(PizzaType::MARGARITA)
                .setSize(PizzaSize::M)
                .setId(200)
                .build();
            kitchen.addPizza(p);
            std::cout << "✗ Should have failed - kitchen is shut down" << std::endl;
        } catch (const std::runtime_error &e) {
            std::cout << "✓ Correctly rejected pizza after shutdown" << std::endl;
        }
        
    } catch (const std::exception &e) {
        std::cout << "✗ Kitchen error: " << e.what() << std::endl;
    }
}

// DEPRECATED: Original test entry point - now using Reception shell
// int main(void)
// {
//     std::cout << "╔════════════════════════════════════════╗" << std::endl;
//     std::cout << "║   PLAZZA - Component Testing Suite     ║" << std::endl;
//     std::cout << "╚════════════════════════════════════════╝" << std::endl;
//     
//     testPizzaBuilder();
//     testStock();
//     testThreading();
//     testProcess();
//     testThreadPool();
//     testCook();
//     testKitchen();
//     
//     std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
//     std::cout << "║        All tests completed!            ║" << std::endl;
//     std::cout << "╚════════════════════════════════════════╝" << std::endl;
//     
//     return 0;
// }
*/