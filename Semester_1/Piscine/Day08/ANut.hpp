#ifndef ANUT_HPP_
#define ANUT_HPP_
#include "AFruit.hpp"

class ANut : public AFruit {
public:
    ANut(const std::string &name, unsigned int vitamins);
    virtual ~ANut() = default;
};
#endif
