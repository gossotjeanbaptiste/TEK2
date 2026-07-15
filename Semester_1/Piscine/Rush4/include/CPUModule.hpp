#ifndef CPUMODULE_HPP_
#define CPUMODULE_HPP_

#include "IModule.hpp"
#include <sstream>
#include <iomanip>

class CPUModule : public Krell::IModule {
private:
    mutable std::string cached_data;
    
public:
    CPUModule() = default;
    virtual ~CPUModule() = default;
    
    std::string getName() const override {
        return "CPU";
    }
    
    void update() override;
    std::string getData() const override;
};

#endif
