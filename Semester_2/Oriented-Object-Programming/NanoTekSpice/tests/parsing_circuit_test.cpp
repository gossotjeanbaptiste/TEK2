/*
** EPITECH PROJECT, 2024
** Epi-Template-C
** File description:
** Main for Epi-Template-CPP
*/

#include <cassert>
#include <iostream>
#include "include/General.hpp"
#include "include/Parser.hpp"
#include "include/Circuit.hpp"

int main(void)
{
    try {
        // Initialize the component factory
        nts::ComponentFactory::initRegistry();
        
        // Test: Parser and Circuit
        std::cout << "=== Test Parser and Circuit ===" << std::endl;
        
        nts::Circuit circuit;
        nts::Parser parser;
        
        std::cout << "Loading circuit from test_circuit.nts..." << std::endl;
        parser.parseFile("tests/files/test_circuit.nts", circuit);
        std::cout << "✓ Circuit loaded successfully!" << std::endl;
        
        std::cout << "\nSetting input values..." << std::endl;
        circuit.setInputValue("in_1", "1");
        circuit.setInputValue("in_2", "0");
        std::cout << "✓ Input values set!" << std::endl;
        
        std::cout << "\nRunning simulation for 3 ticks..." << std::endl;
        for (int i = 0; i < 3; i++) {
            circuit.simulate();
            circuit.display();
            std::cout << std::endl;
        }
        
        std::cout << "=== Test Complete ===" << std::endl;
        return 0;
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}
