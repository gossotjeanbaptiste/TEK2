#ifndef INCLUDED_AND_HPP
#define INCLUDED_AND_HPP

#include <iostream>
#include "../AComponent.hpp"

class And : public AComponent
{
    private:
        std::string _name;
        nts::IComponent *_input1 = nullptr;
        nts::IComponent *_input2 = nullptr;
        std::size_t _input1Pin = 0;
        std::size_t _input2Pin = 0;
        nts::Tristate _output = nts::Undefined;
    
    protected:
    public:
        And(const std::string& name);
        And(const And& other);
        And& operator=(const And& other);
        ~And() = default;

        void display(std::ostream& os = std::cout);
        
        // Implémenter les méthodes virtuelles de IComponent
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const And&);

#endif