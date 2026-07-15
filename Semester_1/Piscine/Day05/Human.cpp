#include "Human.hpp"

Mondas::Human::Human(std::string name, unsigned int iq) : _name(name), _iq(iq) {
    std::cout << name << ": I must save Mondas with my IQ of " << iq << "." << std::endl;
}

Mondas::Human::~Human() { 
    std::cout << _name << ": I failed to save my world..." << std::endl;
}

std::string Mondas::Human::getName() const {
    return _name;
}

unsigned int Mondas::Human::getIq() const {
    return _iq;
}

void Mondas::Human::setIq(unsigned int newIq) {
    if (newIq > _iq) {
        _iq = newIq;
        std::cout << _name << ": My IQ is now " << _iq << "." << std::endl;
    }
}

void Mondas::Human::think() const { 
    std::cout << _name << ": Think, think, think..." << std::endl;
}
