#ifndef RAMMODULE_HPP_
#define RAMMODULE_HPP_

#include "IModule.hpp"
#include <sstream>
#include <iomanip>

class RAMModule : public Krell::IModule {
private:
    mutable std::string cached_data;
    
public:
    RAMModule() = default;
    virtual ~RAMModule() = default;
    
    std::string getName() const override {
        return "RAM";
    }
    
    void update() override;
    std::string getData() const override;
};

#endif
