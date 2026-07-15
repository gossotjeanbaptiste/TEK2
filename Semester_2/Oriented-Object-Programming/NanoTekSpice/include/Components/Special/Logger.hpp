#ifndef INCLUDED_LOGGER_HPP
#define INCLUDED_LOGGER_HPP

#include <fstream>
#include <map>
#include "../AComponent.hpp"

class Logger : public AComponent
{
    private:
        struct Link {
            nts::IComponent* component;
            std::size_t pin;
        };
        
        std::map<std::size_t, Link> _links;
        nts::Tristate _lastClock;
        
    public:
        Logger(const std::string& name);
        ~Logger() = default;
        
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
        
    private:
        nts::Tristate getInput(std::size_t pin);
        void writeCharacter();
};

#endif