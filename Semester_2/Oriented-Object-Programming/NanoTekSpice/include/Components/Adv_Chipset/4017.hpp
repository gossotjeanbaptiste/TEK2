#ifndef INCLUDED_Chip4017_HPP
#define INCLUDED_Chip4017_HPP

#include <iostream>
#include <array>
#include <map>
#include "../AComponent.hpp"

class Chip4017 : public AComponent
{
    private:
        std::string _name;
        unsigned char _counter = 0;
        std::map<std::size_t, std::pair<nts::IComponent*, std::size_t>> _links;
        std::map<std::size_t, nts::Tristate> _pinValues;
    
    protected:
    public:
        Chip4017(const std::string& name);
        Chip4017(const Chip4017& other);
        Chip4017& operator=(const Chip4017& other);
        ~Chip4017() = default;

        void display(std::ostream& os = std::cout);
        
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;

    private:
        nts::Tristate _getPinValue(std::size_t pin);
};

std::ostream& operator<<(std::ostream&, const Chip4017&);

#endif