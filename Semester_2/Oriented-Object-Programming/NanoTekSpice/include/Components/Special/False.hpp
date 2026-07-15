#ifndef INCLUDED_FalseComponent_HPP
#define INCLUDED_FalseComponent_HPP

#include "../AComponent.hpp"

class FalseComponent : public AComponent
{
    public:
        FalseComponent(const std::string& name);
        ~FalseComponent() = default;

        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

#endif