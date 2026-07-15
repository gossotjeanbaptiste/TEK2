#ifndef INCLUDED_Chip4801_HPP
#define INCLUDED_Chip4801_HPP

#include <iostream>
#include <array>
#include <map>
#include "../AComponent.hpp"
class Chip4801 : public AComponent {
    private:
        std::string _name;
        std::array<unsigned char, 256> _memory;
        std::map<std::size_t, std::pair<nts::IComponent*, std::size_t>> _links;
        std::map<std::size_t, nts::Tristate> _pinValues;
        nts::Tristate _getPinValue(std::size_t pin);
        unsigned char _getAddress();
        unsigned char _getDataByte();
        void _setDataByte(unsigned char data);
    protected:
    public:
        Chip4801(const std::string& name);
        Chip4801(const Chip4801& other);
        Chip4801& operator=(const Chip4801& other);
        ~Chip4801() = default;
        void display(std::ostream& os = std::cout);
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Chip4801&);

#endif