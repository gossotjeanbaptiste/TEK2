#ifndef INCLUDED_Chip2716_HPP
#define INCLUDED_Chip2716_HPP

#include <iostream>
#include <vector>
#include <map>
#include "../AComponent.hpp"

class Chip2716 : public AComponent
{
    private:
        struct Link {
            nts::IComponent* component;
            std::size_t pin;
        };
        
        std::string _name;
        std::vector<unsigned char> _memory;
        std::map<std::size_t, Link> _links;
        
        void _loadROM();
        unsigned short _readAddress();
    
    protected:
    public:
        Chip2716(const std::string& name);
        Chip2716(const Chip2716& other);
        Chip2716& operator=(const Chip2716& other);
        ~Chip2716() = default;

        void display(std::ostream& os = std::cout);
        
        // Implémenter les méthodes virtuelles de IComponent
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Chip2716&);

#endif