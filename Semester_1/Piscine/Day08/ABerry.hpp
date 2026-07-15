#pragma once
#ifndef ABERRY_HPP_
    #define ABERRY_HPP_

#include "AFruit.hpp"

class ABerry : public AFruit {
public:
    ABerry(const std::string &name, unsigned int vitamins);
    virtual ~ABerry() = default;
};
#endif