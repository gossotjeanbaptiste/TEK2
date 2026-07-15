#include "Knight.hpp"
#include "Peasant.hpp"

Knight::Knight(const std::string &name, int power) : Peasant(name, power)
{ 
    std::cout << name << " vows to protect the kingdom." << std::endl;
}

int Knight::attack()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return 0; }
    if (power < 10) { std::cout << name << " is out of power." << std::endl; return 0; }
    power -= 10;
    std::cout << name << " strikes with his sword." << std::endl;
    return 20;
}

int Knight::special()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return 0; }
    if (power < 30) { std::cout << name << " is out of power." << std::endl; return 0;  }
    power -= 30;
    std::cout << name << " impales his enemy." << std::endl;
    return 50;
}

void Knight::rest()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return; }
    std::cout << name << " eats." << std::endl;
    power = std::min(power + 50, 100);
}


Knight::~Knight()
{
    std::cout << name << " takes off his armor." << std::endl;
}
