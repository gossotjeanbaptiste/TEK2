#include "Enchanter.hpp"
#include "Peasant.hpp"

Enchanter::Enchanter(const std::string &name, int power) 
    : Peasant(name, power)
{ 
    std::cout << name << " learns magic from his spellbook." << std::endl;
}

int Enchanter::attack()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return 0; }
    std::cout << name << " doesn't know how to fight." << std::endl;
    return 0;
}

int Enchanter::special()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return 0; }
    if (power < 50) { std::cout << name << " is out of power." << std::endl; return 0;  }
    power -= 50;
    std::cout << name << " casts a fireball." << std::endl;
    return 99;
}

void Enchanter::rest()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return; }
    std::cout << name << " meditates." << std::endl;
    power = std::min(power + 100, 100);
}


Enchanter::~Enchanter()
{
    std::cout << name << " closes his spellbook." << std::endl;
}
