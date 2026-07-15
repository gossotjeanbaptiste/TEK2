#include "FruitBox.hpp"
#include <iostream>

FruitBox::FruitBox(unsigned int size)
    : _size(size), _count(0), _head(0)
{
    _fruits = new IFruit *[size];
    for (unsigned int i = 0; i < size; i++)
        _fruits[i] = nullptr;
}

FruitBox::~FruitBox()
{
    for (unsigned int i = 0; i < _count; i++) {
        unsigned int idx = (_head + i) % _size;
        if (_fruits[idx] != nullptr)
            delete _fruits[idx];
    }
    delete[] _fruits;
}

unsigned int FruitBox::getSize() const
{
    return _size;
}

unsigned int FruitBox::nbFruits() const
{
    return _count;
}

bool FruitBox::pushFruit(IFruit *fruit)
{
    if (_count >= _size)
        return false;
    
    // Vérifier que le fruit n'est pas déjà dans la boîte
    for (unsigned int i = 0; i < _count; i++) {
        unsigned int idx = (_head + i) % _size;
        if (_fruits[idx] == fruit)
            return false;
    }
    
    unsigned int tail = (_head + _count) % _size;
    _fruits[tail] = fruit;
    _count++;
    return true;
}

IFruit *FruitBox::popFruit()
{
    if (_count == 0)
        return nullptr;
    
    IFruit *fruit = _fruits[_head];
    _fruits[_head] = nullptr;
    _head = (_head + 1) % _size;
    _count--;
    return fruit;
}

std::ostream &operator<<(std::ostream &os, const FruitBox &box)
{
    os << "[";
    for (unsigned int i = 0; i < box._count; i++) {
        if (i > 0)
            os << ", ";
        unsigned int idx = (box._head + i) % box._size;
        os << *box._fruits[idx];
    }
    os << "]";
    return os;
}
