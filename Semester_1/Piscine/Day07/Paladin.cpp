#include "Paladin.hpp"
#include "Knight.hpp"
#include "Priest.hpp"
#include "Enchanter.hpp"
#include "Peasant.hpp"

Paladin::Paladin(const std::string &name, int power)
    : Peasant(name, power), Knight(name, power), 
    Enchanter(name, power), Priest(name, power)
{
    std::cout << name << " fights for the light." << std::endl;
}

int Paladin::attack()
{
    return Knight::attack();
}

int Paladin::special()
{
    return Enchanter::special();
}

void Paladin::rest()
{
    return Priest::rest();
}

Paladin::~Paladin()
{
    std::cout << name << " is blessed." << std::endl;
}
