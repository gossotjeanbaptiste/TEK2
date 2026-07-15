#include "ABerry.hpp"

ABerry::ABerry(const std::string &name, unsigned int vitamins)
    : AFruit(name, vitamins)
{
    _peeled = true;  // Les baies sont toujours pelées
}
