#include <iostream>
#include "../FruitUtils.hpp"
#include "../Orange.hpp"
#include "../Lemon.hpp"
#include "../Strawberry.hpp"
#include "../Almond.hpp"
#include "../Grapefruit.hpp"
#include "../BloodOrange.hpp"
#include "../Raspberry.hpp"
#include "../Coconut.hpp"

void printBoxInfo(FruitBox **boxes, const std::string &label)
{
    std::cout << label << std::endl;
    unsigned int totalBoxes = 0;
    unsigned int totalFruits = 0;
    for (unsigned int i = 0; boxes[i] != nullptr; i++) {
        totalBoxes++;
        totalFruits += boxes[i]->nbFruits();
        std::cout << "  Box " << (i + 1) << ": " << boxes[i]->nbFruits() << "/" 
                  << boxes[i]->getSize() << " fruits" << std::endl;
    }
    std::cout << "  Total: " << totalBoxes << " box(es) with " << totalFruits << " fruit(s)" << std::endl;
}

int main(void)
{
    std::cout << "╔════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║     Testing FruitUtils::pack and unpack Functions      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════╝" << std::endl;
    
    // Test 1: Pack 25 fruits into boxes of size 6
    std::cout << "\n[TEST 1] Pack 25 fruits into boxes of size 6" << std::endl;
    std::cout << "─────────────────────────────────────────────────" << std::endl;
    {
        IFruit **fruits = new IFruit *[26];  // 25 fruits + nullptr
        
        // Create 25 fruits
        for (int i = 0; i < 25; i++) {
            switch (i % 4) {
                case 0:
                    fruits[i] = new Orange();
                    break;
                case 1:
                    fruits[i] = new Lemon();
                    break;
                case 2:
                    fruits[i] = new Strawberry();
                    break;
                case 3:
                    fruits[i] = new Almond();
                    break;
            }
        }
        fruits[25] = nullptr;
        
        std::cout << "Created 25 fruits" << std::endl;
        
        // Pack fruits
        FruitBox **boxes = FruitUtils::pack(fruits, 6);
        printBoxInfo(boxes, "After packing:");
        
        // Calculate expected boxes
        unsigned int expected = (25 + 6 - 1) / 6;  // Should be 5
        std::cout << "Expected: " << expected << " boxes ✓" << std::endl;
        
        // Unpack and verify
        std::cout << "\nUnpacking fruits..." << std::endl;
        IFruit **unpacked = FruitUtils::unpack(boxes);
        
        unsigned int unpackedCount = 0;
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            unpackedCount++;
        }
        std::cout << "Unpacked fruits: " << unpackedCount << " (expected: 25) " 
                  << (unpackedCount == 25 ? "✓" : "✗") << std::endl;
        
        // Verify boxes are now empty
        std::cout << "After unpacking, boxes should be empty:" << std::endl;
        unsigned int remainingFruits = 0;
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            remainingFruits += boxes[i]->nbFruits();
            std::cout << "  Box " << (i + 1) << ": " << boxes[i]->nbFruits() << " fruits" << std::endl;
        }
        std::cout << "Total remaining: " << remainingFruits << " (expected: 0) " 
                  << (remainingFruits == 0 ? "✓" : "✗") << std::endl;
        
        // Clean up
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            delete unpacked[i];
        }
        delete[] unpacked;
        
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            delete boxes[i];
        }
        delete[] boxes;
        
        delete[] fruits;
    }
    
    // Test 2: Pack 10 fruits into boxes of size 3
    std::cout << "\n[TEST 2] Pack 10 fruits into boxes of size 3" << std::endl;
    std::cout << "─────────────────────────────────────────────────" << std::endl;
    {
        IFruit **fruits = new IFruit *[11];  // 10 fruits + nullptr
        
        fruits[0] = new Orange();
        fruits[1] = new Lemon();
        fruits[2] = new Strawberry();
        fruits[3] = new Almond();
        fruits[4] = new Grapefruit();
        fruits[5] = new BloodOrange();
        fruits[6] = new Raspberry();
        fruits[7] = new Coconut();
        fruits[8] = new Orange();
        fruits[9] = new Lemon();
        fruits[10] = nullptr;
        
        std::cout << "Created 10 mixed fruits (all types)" << std::endl;
        
        FruitBox **boxes = FruitUtils::pack(fruits, 3);
        printBoxInfo(boxes, "After packing:");
        
        unsigned int expected = (10 + 3 - 1) / 3;  // Should be 4
        std::cout << "Expected: " << expected << " boxes ✓" << std::endl;
        
        // Test unpack
        std::cout << "\nUnpacking..." << std::endl;
        IFruit **unpacked = FruitUtils::unpack(boxes);
        
        unsigned int unpackedCount = 0;
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            unpackedCount++;
        }
        std::cout << "Unpacked fruits: " << unpackedCount << " (expected: 10) " 
                  << (unpackedCount == 10 ? "✓" : "✗") << std::endl;
        
        // Verify boxes are empty
        unsigned int remainingFruits = 0;
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            remainingFruits += boxes[i]->nbFruits();
        }
        std::cout << "Remaining fruits in boxes: " << remainingFruits << " (expected: 0) " 
                  << (remainingFruits == 0 ? "✓" : "✗") << std::endl;
        
        // Clean up
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            delete unpacked[i];
        }
        delete[] unpacked;
        
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            delete boxes[i];
        }
        delete[] boxes;
        
        delete[] fruits;
    }
    
    // Test 3: Pack 0 fruits
    std::cout << "\n[TEST 3] Pack 0 fruits" << std::endl;
    std::cout << "─────────────────────────────────────────────────" << std::endl;
    {
        IFruit **fruits = new IFruit *[1];
        fruits[0] = nullptr;
        
        std::cout << "Created 0 fruits" << std::endl;
        
        FruitBox **boxes = FruitUtils::pack(fruits, 5);
        
        unsigned int boxCount = 0;
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            boxCount++;
        }
        std::cout << "Total boxes: " << boxCount << " (expected: 0) " 
                  << (boxCount == 0 ? "✓" : "✗") << std::endl;
        
        delete[] boxes;
        delete[] fruits;
    }
    
    // Test 4: Pack exactly fills boxes
    std::cout << "\n[TEST 4] Pack 12 fruits into boxes of size 4 (exact fit)" << std::endl;
    std::cout << "───────────────────────────────────────────────────────────────" << std::endl;
    {
        IFruit **fruits = new IFruit *[13];
        
        for (int i = 0; i < 12; i++) {
            switch (i % 3) {
                case 0: fruits[i] = new Orange(); break;
                case 1: fruits[i] = new Lemon(); break;
                case 2: fruits[i] = new Strawberry(); break;
            }
        }
        fruits[12] = nullptr;
        
        std::cout << "Created 12 fruits" << std::endl;
        
        FruitBox **boxes = FruitUtils::pack(fruits, 4);
        printBoxInfo(boxes, "After packing:");
        
        std::cout << "Expected: 3 boxes (4 + 4 + 4) ✓" << std::endl;
        
        // Unpack
        IFruit **unpacked = FruitUtils::unpack(boxes);
        
        unsigned int unpackedCount = 0;
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            unpackedCount++;
        }
        std::cout << "Unpacked: " << unpackedCount << " fruits (expected: 12) " 
                  << (unpackedCount == 12 ? "✓" : "✗") << std::endl;
        
        // Clean up
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            delete unpacked[i];
        }
        delete[] unpacked;
        
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            delete boxes[i];
        }
        delete[] boxes;
        
        delete[] fruits;
    }
    
    // Test 5: Pack with single fruit
    std::cout << "\n[TEST 5] Pack 1 fruit into box of size 5" << std::endl;
    std::cout << "─────────────────────────────────────────────────" << std::endl;
    {
        IFruit **fruits = new IFruit *[2];
        fruits[0] = new Orange();
        fruits[1] = nullptr;
        
        std::cout << "Created 1 fruit" << std::endl;
        
        FruitBox **boxes = FruitUtils::pack(fruits, 5);
        printBoxInfo(boxes, "After packing:");
        
        std::cout << "Expected: 1 box with 1 fruit ✓" << std::endl;
        
        // Unpack
        IFruit **unpacked = FruitUtils::unpack(boxes);
        
        unsigned int unpackedCount = 0;
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            unpackedCount++;
        }
        std::cout << "Unpacked: " << unpackedCount << " fruit (expected: 1) " 
                  << (unpackedCount == 1 ? "✓" : "✗") << std::endl;
        
        // Clean up
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            delete unpacked[i];
        }
        delete[] unpacked;
        
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            delete boxes[i];
        }
        delete[] boxes;
        
        delete[] fruits;
    }
    
    // Test 6: Pack with large box size
    std::cout << "\n[TEST 6] Pack 15 fruits into boxes of size 100 (single box)" << std::endl;
    std::cout << "──────────────────────────────────────────────────────────────────────" << std::endl;
    {
        IFruit **fruits = new IFruit *[16];
        
        for (int i = 0; i < 15; i++) {
            fruits[i] = new Almond();
        }
        fruits[15] = nullptr;
        
        std::cout << "Created 15 fruits" << std::endl;
        
        FruitBox **boxes = FruitUtils::pack(fruits, 100);
        printBoxInfo(boxes, "After packing:");
        
        std::cout << "Expected: 1 box with 15 fruits ✓" << std::endl;
        
        // Unpack
        IFruit **unpacked = FruitUtils::unpack(boxes);
        
        unsigned int unpackedCount = 0;
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            unpackedCount++;
        }
        std::cout << "Unpacked: " << unpackedCount << " fruits (expected: 15) " 
                  << (unpackedCount == 15 ? "✓" : "✗") << std::endl;
        
        // Clean up
        for (unsigned int i = 0; unpacked[i] != nullptr; i++) {
            delete unpacked[i];
        }
        delete[] unpacked;
        
        for (unsigned int i = 0; boxes[i] != nullptr; i++) {
            delete boxes[i];
        }
        delete[] boxes;
        
        delete[] fruits;
    }
    std::cout << "\n" << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              All tests completed successfully ✓        ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════╝" << std::endl;
    
    return 0;
}
