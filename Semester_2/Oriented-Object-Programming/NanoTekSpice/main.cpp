/*
** EPITECH PROJECT, 2024
** Epi-Template-C
** File description:
** Main for Epi-Template-CPP
*/

#include <iostream>
#include <memory>
#include "include/Shell.hpp"
#include "include/Circuit.hpp"
#include "include/Parser.hpp"
#include "include/Components/ComponentFactory.hpp"

/**
 * @brief Main entry point for the NanoTekSpice circuit simulator application.
 *
 * Initializes the component factory registry, parses a circuit definition file,
 * and launches an interactive shell for circuit manipulation and simulation.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments array. argv[1] should contain the path
 *             to the circuit definition file to be parsed.
 *
 * @return 0 on successful execution, 84 on error (invalid arguments or exception).
 *
 * @throws std::exception Caught and logged to stderr with error details.
 *
 * @note Requires exactly 2 arguments: program name and circuit file path.
 *       Usage: program_name <circuit_file>
 */
int main(int argc, char *argv[])
{
    std::remove("./log.bin");
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <circuit_file>" << std::endl;
        return 84;
    }
    try {
        nts::ComponentFactory::initRegistry();
        auto circuit = std::make_unique<nts::Circuit>();
        nts::Parser parser;
        parser.parseFile(argv[1], *circuit);
        Shell shell;
        shell.setCircuit(std::move(circuit));
        shell.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
}

