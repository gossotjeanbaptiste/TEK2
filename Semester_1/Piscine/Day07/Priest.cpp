#include "Priest.hpp"
#include "Enchanter.hpp"
#include "Peasant.hpp"

Priest::Priest(const std::string &name, int power)
    : Peasant(name, power), Enchanter(name, power)
{
    std::cout << name << " enters in the order." << std::endl;
}

void Priest::rest()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return; }
    std::cout << name << " prays." << std::endl;
    power = std::min(power + 100, 100);
    hp = std::min(power + 100, 100);
}

Priest::~Priest()
{
    std::cout << name << " finds peace." << std::endl;
}