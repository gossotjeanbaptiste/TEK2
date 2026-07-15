#include "Peasant.hpp"
#include "IPotion.hpp"
#include <algorithm>

Peasant::Peasant(const std::string &name, int power) 
    :name(name), power(std::min(power, 100)), hp(100)
{
    std::cout << name << " goes for an adventure." << std::endl;
}

const std::string &Peasant::getName() const
{
    return name;
}

int Peasant::getPower() const
{
    return power;
}

int Peasant::getHp() const
{
    return hp;
}

int Peasant::attack()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return 0; }
    if (power < 10) { std::cout << name << " is out of power." << std::endl; return 0; }
    power -= 10;
    std::cout << name << " tosses a stone." << std::endl;
    return 5;
}

int Peasant::special()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return 0; }
    std::cout << name << " doesn't know any special move." << std::endl;
    return 0;
}

void Peasant::rest()
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return; }
    std::cout << name << " takes a nap." << std::endl;
    power = std::min(power + 30, 100);
}

void Peasant::damage(int dmg)
{
    if (hp == 0) { std::cout << name << " is out of combat." << std::endl; return; }
    hp -= dmg;
    if (hp < 0) hp = 0;
    if (hp == 0) {
        std::cout << name << " is out of combat." << std::endl;
    } else {
        std::cout << name << " takes " << dmg << " damage." << std::endl;
    }
}

void Peasant::drink(const IPotion &potion)
{
    potion.applyEffect(*this);
}

void Peasant::addHp(int amount)
{
    hp = std::min(hp + amount, 100);
}

void Peasant::addPower(int amount)
{
    power = std::min(power + amount, 100);
}

Peasant::~Peasant()
{
    std::cout << name << " is back to his crops." << std::endl;
}

UnknownPotion::UnknownPotion()
{
}

UnknownPotion::~UnknownPotion()
{
}

void UnknownPotion::applyEffect(ICharacter &character) const
{
    std::cout << character.getName() << " drinks a mysterious potion." << std::endl;
}