#ifndef INCLUDED_OUTPUT_HPP
#define INCLUDED_OUTPUT_HPP

#include "../AComponent.hpp"

class Output : public AComponent
{
    private:
        struct Link {
            nts::IComponent* component;
            std::size_t pin;
        };
        
        Link _link;
        
    public:
        Output(const std::string& name);
        ~Output() = default;
        
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

#endif