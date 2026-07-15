#ifndef INCLUDED_TrueComponent_HPP
#define INCLUDED_TrueComponent_HPP

#include "../AComponent.hpp"

class TrueComponent : public AComponent
{
    public:
        TrueComponent(const std::string& name);
        ~TrueComponent() = default;

        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

#endif