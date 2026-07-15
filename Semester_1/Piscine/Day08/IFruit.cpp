#include "IFruit.hpp"
#include <iomanip>

std::ostream &operator<<(std::ostream &os, const IFruit &fruit)
{
    os << "{ \"name\": \"" << fruit.getName() 
       << "\", \"vitamins\": " << fruit.getVitamins()
       << ", \"peeled\": " << (fruit.isPeeled() ? "true" : "false") << " }";
    return os;
}