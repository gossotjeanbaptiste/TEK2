/*
** EPITECH PROJECT, 2024
** NanoTekSpice
** File description:
** NOT Gate Test
*/

#include <cassert>
#include <iostream>
#include "../include/General.hpp"
#include "../include/Parser.hpp"
#include "../include/Circuit.hpp"

int main(void)
{
    try {
        // Initialize the component factory
        nts::ComponentFactory::initRegistry();
        
        std::cout << "=== Test NOT Gate Logic ===" << std::endl << std::endl;
        
        nts::Parser parser;
        
        // Test 1: NOT(1) = 0
        std::cout << "Test 1: NOT(1) should output 0" << std::endl;
        nts::Circuit circuit1;
        parser.parseFile("tests/files/test_not.nts", circuit1);
        circuit1.setInputValue("in", "1");
        circuit1.simulate();
        
        auto gate1 = circuit1.getComponent("gate");
        auto in1 = circuit1.getComponent("in");
        nts::Tristate input1 = in1->compute(1);
        nts::Tristate output1 = gate1->compute(2);
        
        std::cout << "  in=" << nts::tristateToString(input1);
        std::cout << " => output=" << nts::tristateToString(output1) << std::endl;
        if (output1 == nts::False) {
            std::cout << "  ✓ PASS: NOT 1 = 0" << std::endl;
        } else {
            std::cout << "  ✗ FAIL: Expected 0, got " << nts::tristateToString(output1) << std::endl;
        }
        
        // Test 2: NOT(0) = 1
        std::cout << std::endl << "Test 2: NOT(0) should output 1" << std::endl;
        nts::Circuit circuit2;
        parser.parseFile("tests/files/test_not.nts", circuit2);
        circuit2.setInputValue("in", "0");
        circuit2.simulate();
        
        auto gate2 = circuit2.getComponent("gate");
        auto in2 = circuit2.getComponent("in");
        nts::Tristate input2 = in2->compute(1);
        nts::Tristate output2 = gate2->compute(2);
        
        std::cout << "  in=" << nts::tristateToString(input2);
        std::cout << " => output=" << nts::tristateToString(output2) << std::endl;
        if (output2 == nts::True) {
            std::cout << "  ✓ PASS: NOT 0 = 1" << std::endl;
        } else {
            std::cout << "  ✗ FAIL: Expected 1, got " << nts::tristateToString(output2) << std::endl;
        }
        
        // Test 3: NOT(U) = U
        std::cout << std::endl << "Test 3: NOT(U) should output U" << std::endl;
        nts::Circuit circuit3;
        parser.parseFile("tests/files/test_not.nts", circuit3);
        // Don't set the input, leaving it undefined (U)
        circuit3.simulate();
        
        auto gate3 = circuit3.getComponent("gate");
        auto in3 = circuit3.getComponent("in");
        nts::Tristate input3 = in3->compute(1);
        nts::Tristate output3 = gate3->compute(2);
        
        std::cout << "  in=" << nts::tristateToString(input3);
        std::cout << " => output=" << nts::tristateToString(output3) << std::endl;
        if (output3 == nts::Undefined) {
            std::cout << "  ✓ PASS: NOT U = U" << std::endl;
        } else {
            std::cout << "  ✗ FAIL: Expected U, got " << nts::tristateToString(output3) << std::endl;
        }
        
        std::cout << std::endl << "=== NOT Gate Tests Complete ===" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}
