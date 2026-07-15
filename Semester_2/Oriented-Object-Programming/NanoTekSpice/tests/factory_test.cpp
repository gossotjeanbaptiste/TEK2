/*
** EPITECH PROJECT, 2024
** Epi-Template-C
** File description:
** Main for Epi-Template-CPP
*/

#include <cassert>
#include "../include/General.hpp"

int main() {
    nts::ComponentFactory::initRegistry();
    
    std::cout << "=== TESTING COMPONENT FACTORY ===" << std::endl;
    std::cout << std::endl;
    
    // Gates
    std::cout << "--- GATES ---" << std::endl;
    nts::IComponent* and_gate = nts::ComponentFactory::create(nts::ComponentType::AND, "AND_1");
    std::cout << "✓ AND gate created (AND_1)" << std::endl;
    delete and_gate;
    
    nts::IComponent* or_gate = nts::ComponentFactory::create(nts::ComponentType::OR, "OR_1");
    std::cout << "✓ OR gate created (OR_1)" << std::endl;
    delete or_gate;
    
    nts::IComponent* not_gate = nts::ComponentFactory::create(nts::ComponentType::NOT, "NOT_1");
    std::cout << "✓ NOT gate created (NOT_1)" << std::endl;
    delete not_gate;
    
    nts::IComponent* xor_gate = nts::ComponentFactory::create(nts::ComponentType::XOR, "XOR_1");
    std::cout << "✓ XOR gate created (XOR_1)" << std::endl;
    delete xor_gate;
    
    std::cout << std::endl;
    
    // Chipsets simples
    std::cout << "--- SIMPLE CHIPSETS ---" << std::endl;
    nts::IComponent* chip_4001 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4001, "4001_1");
    std::cout << "✓ Chip 4001 NOR created (4001_1)" << std::endl;
    delete chip_4001;
    
    nts::IComponent* chip_4011 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4011, "4011_1");
    std::cout << "✓ Chip 4011 NAND created (4011_1)" << std::endl;
    delete chip_4011;
    
    nts::IComponent* chip_4030 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4030, "4030_1");
    std::cout << "✓ Chip 4030 XOR created (4030_1)" << std::endl;
    delete chip_4030;
    
    nts::IComponent* chip_4069 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4069, "4069_1");
    std::cout << "✓ Chip 4069 NOT created (4069_1)" << std::endl;
    delete chip_4069;
    
    nts::IComponent* chip_4071 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4071, "4071_1");
    std::cout << "✓ Chip 4071 OR created (4071_1)" << std::endl;
    delete chip_4071;
    
    nts::IComponent* chip_4081 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4081, "4081_1");
    std::cout << "✓ Chip 4081 AND created (4081_1)" << std::endl;
    delete chip_4081;
    
    std::cout << std::endl;
    
    // Chipsets avancés
    std::cout << "--- ADVANCED CHIPSETS ---" << std::endl;
    nts::IComponent* chip_2716 = nts::ComponentFactory::create(nts::ComponentType::CHIP_2716, "2716_1");
    std::cout << "✓ Chip 2716 ROM created (2716_1)" << std::endl;
    delete chip_2716;
    
    nts::IComponent* chip_4008 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4008, "4008_1");
    std::cout << "✓ Chip 4008 Adder created (4008_1)" << std::endl;
    delete chip_4008;
    
    nts::IComponent* chip_4013 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4013, "4013_1");
    std::cout << "✓ Chip 4013 Flip-Flop created (4013_1)" << std::endl;
    delete chip_4013;
    
    nts::IComponent* chip_4017 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4017, "4017_1");
    std::cout << "✓ Chip 4017 Johnson Counter created (4017_1)" << std::endl;
    delete chip_4017;
    
    nts::IComponent* chip_4040 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4040, "4040_1");
    std::cout << "✓ Chip 4040 Counter created (4040_1)" << std::endl;
    delete chip_4040;
    
    nts::IComponent* chip_4094 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4094, "4094_1");
    std::cout << "✓ Chip 4094 Shift Register created (4094_1)" << std::endl;
    delete chip_4094;
    
    nts::IComponent* chip_4512 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4512, "4512_1");
    std::cout << "✓ Chip 4512 Selector created (4512_1)" << std::endl;
    delete chip_4512;
    
    nts::IComponent* chip_4514 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4514, "4514_1");
    std::cout << "✓ Chip 4514 Decoder created (4514_1)" << std::endl;
    delete chip_4514;
    
    nts::IComponent* chip_4801 = nts::ComponentFactory::create(nts::ComponentType::CHIP_4801, "4801_1");
    std::cout << "✓ Chip 4801 RAM created (4801_1)" << std::endl;
    delete chip_4801;
    
    std::cout << std::endl;
    
    // Special components
    std::cout << "--- SPECIAL COMPONENTS ---" << std::endl;
    nts::IComponent* clock = nts::ComponentFactory::create(nts::ComponentType::CLOCK, "CLK_1");
    std::cout << "✓ Clock created (CLK_1)" << std::endl;
    delete clock;
    
    nts::IComponent* input = nts::ComponentFactory::create(nts::ComponentType::INPUT, "INPUT_1");
    std::cout << "✓ Input created (INPUT_1)" << std::endl;
    delete input;
    
    nts::IComponent* output = nts::ComponentFactory::create(nts::ComponentType::OUTPUT, "OUTPUT_1");
    std::cout << "✓ Output created (OUTPUT_1)" << std::endl;
    delete output;
    
    nts::IComponent* true_comp = nts::ComponentFactory::create(nts::ComponentType::TRUE_COMPONENT, "TRUE_1");
    std::cout << "✓ True created (TRUE_1)" << std::endl;
    delete true_comp;
    
    nts::IComponent* false_comp = nts::ComponentFactory::create(nts::ComponentType::FALSE_COMPONENT, "FALSE_1");
    std::cout << "✓ False created (FALSE_1)" << std::endl;
    delete false_comp;
    
    nts::IComponent* logger = nts::ComponentFactory::create(nts::ComponentType::LOGGER, "LOGGER_1");
    std::cout << "✓ Logger created (LOGGER_1)" << std::endl;
    delete logger;
    
    std::cout << std::endl;
    std::cout << "=== ALL COMPONENTS CREATED SUCCESSFULLY ===" << std::endl;
    
    return 0;
}