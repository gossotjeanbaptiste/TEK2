#ifndef INCLUDED_Chip4013_HPP
#define INCLUDED_Chip4013_HPP

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include "../AComponent.hpp"

class Chip4013 : public AComponent
{
    private:
        std::string _name;
        nts::Tristate _Q1;
        nts::Tristate _Q2;
        nts::Tristate _prevClk1;
        nts::Tristate _prevClk2;
        std::map<std::size_t, std::pair<nts::IComponent*, std::size_t>> _links;
        std::map<std::size_t, nts::Tristate> _pinValues;
        
        nts::Tristate _getPinValue(std::size_t pin);
        bool _isRisingEdge(nts::Tristate currentClk, nts::Tristate prevClk);
    
    protected:
    public:
        Chip4013(const std::string& name);
        Chip4013(const Chip4013& other);
        Chip4013& operator=(const Chip4013& other);
        ~Chip4013() = default;

        void display(std::ostream& os = std::cout) const;
        
        // Implémenter les méthodes virtuelles de IComponent
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Chip4013&);

#endif