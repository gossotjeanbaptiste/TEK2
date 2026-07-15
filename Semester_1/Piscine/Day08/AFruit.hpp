#pragma once
#ifndef AFRUIT_HPP_
#define AFRUIT_HPP_
#include "IFruit.hpp"

class AFruit : public IFruit {
protected:
    std::string _name;
    unsigned int _vitamins;
    bool _peeled;
    
public:
    AFruit(const std::string &name, unsigned int vitamins);
    virtual ~AFruit() = default;
    
    unsigned int getVitamins() const override;
    std::string getName() const override;
    bool isPeeled() const override;
    void peel() override;
};
#endif