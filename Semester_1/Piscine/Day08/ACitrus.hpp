
#ifndef ACITRUS_HPP_
#define ACITRUS_HPP_
#include "AFruit.hpp"

class ACitrus : public AFruit {
public:
    ACitrus(const std::string &name, unsigned int vitamins);
    virtual ~ACitrus() = default;
};
#endif