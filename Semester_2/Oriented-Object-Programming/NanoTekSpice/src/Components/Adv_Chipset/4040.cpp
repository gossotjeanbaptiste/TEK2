#include "../../../include/Components/Adv_Chipset/4040.hpp"

Chip4040::Chip4040(const std::string& name) : _name(name)
{
    // Constructor
}

Chip4040::Chip4040(const Chip4040& other) : _name(other._name)
{
    // Copy constructor
}

Chip4040& Chip4040::operator=(const Chip4040& other)
{
    (void)other;
    return *this;
}

void Chip4040::simulate(std::size_t tick)
{
    (void)tick;
    // To implement
}

nts::Tristate Chip4040::compute(std::size_t pin)
{
    (void)pin;
    return nts::Undefined;
}

void Chip4040::setLink(std::size_t pin, nts::IComponent &other, std::size_t otherPin)
{
    (void)pin;
    (void)other;
    (void)otherPin;
    // To implement
}

void Chip4040::display(std::ostream& os)
{
    (void)os;
    // Display action
}
