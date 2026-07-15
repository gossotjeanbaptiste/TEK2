#include <iostream>
#include <string>
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/VirtualMachine.hpp"

void displayUsage(const std::string& programName) {
    std::cout << "usage: " << programName << " [options]" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "  -h, --help    Show this help" << std::endl;
    std::cout << "  -b, --bonus   Add any flag you want for bonuses!" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Handle command-line arguments
        if (argc > 1) {
            std::string arg = argv[1];
            
            if (arg == "-h" || arg == "--help") {
                displayUsage(argv[0]);
                return 0;
            }
            
            if (arg == "-b" || arg == "--bonus") {
                // Bonus feature handling can be added here
                if (argc > 2) {
                    // If there's a file after bonus flag, read it
                    Lexer lexer = Lexer::fromFile(argv[2]);
                    auto tokens = lexer.tokenize();
                    Parser parser(tokens);
                    auto instructions = parser.parse();
                    VirtualMachine vm;
                    vm.run(instructions);
                } else {
                    // Read from stdin
                    Lexer lexer = Lexer::fromStdin();
                    auto tokens = lexer.tokenize();
                    Parser parser(tokens);
                    auto instructions = parser.parse();
                    VirtualMachine vm;
                    vm.run(instructions);
                }
                return 0;
            }
            
            // Treat argument as filename
            Lexer lexer = Lexer::fromFile(arg);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);
            auto instructions = parser.parse();
            VirtualMachine vm;
            vm.run(instructions);
            return 0;
        }
        
        // No arguments: read from stdin
        Lexer lexer = Lexer::fromStdin();
        auto tokens = lexer.tokenize();
        
        if (tokens.empty()) {
            std::cerr << "error: empty input" << std::endl;
            return 84;
        }
        
        Parser parser(tokens);
        auto instructions = parser.parse();
        
        VirtualMachine vm;
        vm.run(instructions);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 84;
    }
}
