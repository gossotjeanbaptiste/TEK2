#ifndef INCLUDED_Xor_HPP
#define INCLUDED_Xor_HPP

#include <iostream>
#include "../AComponent.hpp"

class Xor : public AComponent
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
        Xor(const std::string& name);
        Xor(const Xor& other);
        Xor& operator=(const Xor& other);
        ~Xor() = default;

        void display(std::ostream& os = std::cout);
        
        // Implémenter les méthodes virtuelles de IComponent
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Xor&);

#endif