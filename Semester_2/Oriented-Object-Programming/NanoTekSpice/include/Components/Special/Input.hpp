#ifndef INCLUDED_Input_HPP
#define INCLUDED_Input_HPP

#include <iostream>
#include "../AComponent.hpp"

class Input : public AComponent
{
    private:
        std::string _name;
        nts::Tristate _value = nts::Undefined;
    
    protected:
    public:
        Input(const std::string& name);
        Input(const Input& other);
        Input& operator=(const Input& other);
        ~Input() = default;

        void setValue(nts::Tristate value) { _value = value; }
        void display(std::ostream& os = std::cout);
        
        // Implémenter les méthodes virtuelles de IComponent
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Input&);

#endif