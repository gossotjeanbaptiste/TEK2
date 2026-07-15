#ifndef INCLUDED_Chip4040_HPP
#define INCLUDED_Chip4040_HPP

#include <iostream>
#include "../AComponent.hpp"

class Chip4040 : public AComponent
{
    private:
        std::string _name;
    
    protected:
    public:
        Chip4040(const std::string& name);
        Chip4040(const Chip4040& other);
        Chip4040& operator=(const Chip4040& other);
        ~Chip4040() = default;

        void display(std::ostream& os = std::cout);
        
        // Implémenter les méthodes virtuelles de IComponent
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Chip4040&);

#endif