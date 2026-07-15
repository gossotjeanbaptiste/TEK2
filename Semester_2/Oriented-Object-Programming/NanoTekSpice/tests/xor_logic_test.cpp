/*
** EPITECH PROJECT, 2024
** NanoTekSpice
** File description:
** XOR Gate Test
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
        
        std::cout << "=== Test XOR Gate Logic ===" << std::endl << std::endl;
        
        nts::Parser parser;
        
        // Test 1: XOR(1, 1) = 0 (same inputs)
        std::cout << "Test 1: XOR(1, 1) should output 0 (same)" << std::endl;
        nts::Circuit circuit1;
        parser.parseFile("tests/files/test_xor.nts", circuit1);
        circuit1.setInputValue("in_x", "1");
        circuit1.setInputValue("in_y", "1");
        circuit1.simulate();
        
        auto gate1 = circuit1.getComponent("gate_xor");
        auto in_x1 = circuit1.getComponent("in_x");
        auto in_y1 = circuit1.getComponent("in_y");
        nts::Tristate input_x1 = in_x1->compute(1);
        nts::Tristate input_y1 = in_y1->compute(1);
        nts::Tristate output1 = gate1->compute(3);
        
        std::cout << "  in_x=" << nts::tristateToString(input_x1);
        std::cout << ", in_y=" << nts::tristateToString(input_y1);
        std::cout << " => output=" << nts::tristateToString(output1) << std::endl;
        if (output1 == nts::False) {
            std::cout << "  ✓ PASS: 1 XOR 1 = 0" << std::endl;
        } else {
            std::cout << "  ✗ FAIL: Expected 0, got " << nts::tristateToString(output1) << std::endl;
        }
        
        // Test 2: XOR(1, 0) = 1 (different inputs)
        std::cout << std::endl << "Test 2: XOR(1, 0) should output 1 (different)" << std::endl;
        nts::Circuit circuit2;
        parser.parseFile("tests/files/test_xor.nts", circuit2);
        circuit2.setInputValue("in_x", "1");
        circuit2.setInputValue("in_y", "0");
        circuit2.simulate();
        
        auto gate2 = circuit2.getComponent("gate_xor");
        auto in_x2 = circuit2.getComponent("in_x");
        auto in_y2 = circuit2.getComponent("in_y");
        nts::Tristate input_x2 = in_x2->compute(1);
        nts::Tristate input_y2 = in_y2->compute(1);
        nts::Tristate output2 = gate2->compute(3);
        
        std::cout << "  in_x=" << nts::tristateToString(input_x2);
        std::cout << ", in_y=" << nts::tristateToString(input_y2);
        std::cout << " => output=" << nts::tristateToString(output2) << std::endl;
        if (output2 == nts::True) {
            std::cout << "  ✓ PASS: 1 XOR 0 = 1" << std::endl;
        } else {
            std::cout << "  ✗ FAIL: Expected 1, got " << nts::tristateToString(output2) << std::endl;
        }
        
        // Test 3: XOR(0, 0) = 0 (same inputs)
        std::cout << std::endl << "Test 3: XOR(0, 0) should output 0 (same)" << std::endl;
        nts::Circuit circuit3;
        parser.parseFile("tests/files/test_xor.nts", circuit3);
        circuit3.setInputValue("in_x", "0");
        circuit3.setInputValue("in_y", "0");
        circuit3.simulate();
        
        auto gate3 = circuit3.getComponent("gate_xor");
        auto in_x3 = circuit3.getComponent("in_x");
        auto in_y3 = circuit3.getComponent("in_y");
        nts::Tristate input_x3 = in_x3->compute(1);
        nts::Tristate input_y3 = in_y3->compute(1);
        nts::Tristate output3 = gate3->compute(3);
        
        std::cout << "  in_x=" << nts::tristateToString(input_x3);
        std::cout << ", in_y=" << nts::tristateToString(input_y3);
        std::cout << " => output=" << nts::tristateToString(output3) << std::endl;
        if (output3 == nts::False) {
            std::cout << "  ✓ PASS: 0 XOR 0 = 0" << std::endl;
        } else {
            std::cout << "  ✗ FAIL: Expected 0, got " << nts::tristateToString(output3) << std::endl;
        }
        
        // Test 4: XOR(0, 1) = 1 (different inputs)
        std::cout << std::endl << "Test 4: XOR(0, 1) should output 1 (different)" << std::endl;
        nts::Circuit circuit4;
        parser.parseFile("tests/files/test_xor.nts", circuit4);
        circuit4.setInputValue("in_x", "0");
        circuit4.setInputValue("in_y", "1");
        circuit4.simulate();
        
        auto gate4 = circuit4.getComponent("gate_xor");
        auto in_x4 = circuit4.getComponent("in_x");
        auto in_y4 = circuit4.getComponent("in_y");
        nts::Tristate input_x4 = in_x4->compute(1);
        nts::Tristate input_y4 = in_y4->compute(1);
        nts::Tristate output4 = gate4->compute(3);
        
        std::cout << "  in_x=" << nts::tristateToString(input_x4);
        std::cout << ", in_y=" << nts::tristateToString(input_y4);
        std::cout << " => output=" << nts::tristateToString(output4) << std::endl;
        if (output4 == nts::True) {
            std::cout << "  ✓ PASS: 0 XOR 1 = 1" << std::endl;
        } else {
            std::cout << "  ✗ FAIL: Expected 1, got " << nts::tristateToString(output4) << std::endl;
        }
        
        std::cout << std::endl << "=== XOR Gate Tests Complete ===" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}
