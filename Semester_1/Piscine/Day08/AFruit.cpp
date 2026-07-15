#include "AFruit.hpp"

AFruit::AFruit(const std::string &name, unsigned int vitamins)
    : _name(name), _vitamins(vitamins), _peeled(false)
{}

unsigned int AFruit::getVitamins() const
{
    return _peeled ? _vitamins : 0;
}

std::string AFruit::getName() const
{
    return _name;
}

bool AFruit::isPeeled() const
{
    return _peeled;
}

void AFruit::peel()
{
    _peeled = true;
}