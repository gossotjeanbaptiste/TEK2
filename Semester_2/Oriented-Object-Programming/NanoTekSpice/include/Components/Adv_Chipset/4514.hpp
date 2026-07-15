#ifndef INCLUDED_Chip4514_HPP
#define INCLUDED_Chip4514_HPP

#include <iostream>
#include <array>
#include <map>
#include "../AComponent.hpp"

class Chip4514 : public AComponent
{
    private:
        std::string _name;
        std::map<std::size_t, std::pair<nts::IComponent*, std::size_t>> _links;
        std::map<std::size_t, nts::Tristate> _pinValues;
    
    protected:
    public:
        Chip4514(const std::string& name);
        Chip4514(const Chip4514& other);
        Chip4514& operator=(const Chip4514& other);
        ~Chip4514() = default;

        void display(std::ostream& os = std::cout);
        
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;

    private:
        nts::Tristate _getPinValue(std::size_t pin);
        unsigned char _getAddress();
};

std::ostream& operator<<(std::ostream&, const Chip4514&);

#endif