#ifndef INCLUDED_Not_HPP
#define INCLUDED_Not_HPP

#include <iostream>
#include "../AComponent.hpp"

class Not : public AComponent
{
    private:
        std::string _name;                    /**< Component name identifier */
        nts::IComponent *_input = nullptr;    /**< Pointer to input component */
        std::size_t _inputPin = 0;            /**< Pin number of input component */
        nts::Tristate _output = nts::Undefined; /**< Current output state */
    
    protected:
    public:
        Not(const std::string& name);
        Not(const Not& other);
        Not& operator=(const Not& other);
        ~Not() = default;

        void display(std::ostream& os = std::cout);
        
        // Implémenter les méthodes virtuelles de IComponent
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Not&);

#endif