#ifndef FRUITUTILS_HPP_
#define FRUITUTILS_HPP_

#include "FruitBox.hpp"

class FruitUtils {
public:
    static void sort(FruitBox &unsorted, FruitBox &lemon, FruitBox &citrus, FruitBox &berry);
    static FruitBox **pack(IFruit **fruits, unsigned int boxSize);
    static IFruit **unpack(FruitBox **fruitBoxes);
};

#endif
