#ifndef FRUITBOX_HPP_
#define FRUITBOX_HPP_

#include "IFruit.hpp"
#include "AFruit.hpp"
#include <iostream>

class FruitBox {
private:
    IFruit **_fruits;
    unsigned int _size;
    unsigned int _count;
    unsigned int _head;

    // Empêcher la copie
    FruitBox(const FruitBox &);
    FruitBox &operator=(const FruitBox &);

public:
    FruitBox(unsigned int size);
    ~FruitBox();

    unsigned int getSize() const;
    unsigned int nbFruits() const;
    bool pushFruit(IFruit *fruit);
    IFruit *popFruit();

    friend std::ostream &operator<<(std::ostream &os, const FruitBox &box);
};

std::ostream &operator<<(std::ostream &os, const FruitBox &box);

#endif
