#ifndef INCLUDED_Chip4512_HPP
#define INCLUDED_Chip4512_HPP

#include <iostream>
#include <array>
#include <map>
#include "../AComponent.hpp"

class Chip4512 : public AComponent
{
    private:
        std::string _name;
        std::map<std::size_t, std::pair<nts::IComponent*, std::size_t>> _links;
        nts::Tristate _outputValue = nts::Undefined;
    
    protected:
    public:
        Chip4512(const std::string& name);
        Chip4512(const Chip4512& other);
        Chip4512& operator=(const Chip4512& other);
        ~Chip4512() = default;

        void display(std::ostream& os = std::cout);
        
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;

    private:
        nts::Tristate _getPinValue(std::size_t pin);
        unsigned char _getSelectCode();
        nts::Tristate _getDataInput(unsigned char index);
};

std::ostream& operator<<(std::ostream&, const Chip4512&);

#endif