#ifndef INCLUDED_SHELL_HPP
    #define INCLUDED_SHELL_HPP

#include <iostream>
#include <string>
#include <memory>
#include "Circuit.hpp"

class Shell
{
    private:
        std::unique_ptr<nts::Circuit> _circuit;
        bool _running;

        void handleCommand(const std::string& command);
        void executeDisplay();
        void executeSetInput(const std::string& input);
        void executeSimulate();
        void executeLoop();
        
    protected:
    public:
        Shell();
        Shell(const Shell& other);
        Shell& operator=(const Shell& other);
        ~Shell() = default;

        void setCircuit(std::unique_ptr<nts::Circuit>&& circuit);
        void run();
        void display(std::ostream& os = std::cout);
};

#endif
