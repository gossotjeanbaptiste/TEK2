#ifndef BATTERYMODULE_HPP_
#define BATTERYMODULE_HPP_

#include "IModule.hpp"
#include <sstream>
#include <iomanip>

class BatteryModule : public Krell::IModule {
private:
    mutable std::string cached_data;
    
public:
    BatteryModule() = default;
    virtual ~BatteryModule() = default;
    
    std::string getName() const override {
        return "Battery";
    }
    
    void update() override;
    std::string getData() const override;
};

#endif
