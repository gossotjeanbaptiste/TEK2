#ifndef INCLUDED_Clock_HPP
#define INCLUDED_Clock_HPP

#include <iostream>
#include "../AComponent.hpp"

class Clock : public AComponent
{
    private:
        std::string _name;
        nts::Tristate _value = nts::Undefined;
    protected:
    public:
        Clock(const std::string& name);
        Clock(const Clock& other);
        Clock& operator=(const Clock& other);
        ~Clock() = default;

        void display(std::ostream& os = std::cout);
        void simulate(std::size_t tick) override;
        nts::Tristate compute(std::size_t pin) override;
        void setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin) override;
};

std::ostream& operator<<(std::ostream&, const Clock&);

#endif