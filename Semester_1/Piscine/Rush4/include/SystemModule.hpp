#ifndef SYSTEMMODULE_HPP_
#define SYSTEMMODULE_HPP_

#include "IModule.hpp"
#include <sstream>
#include <iomanip>

class SystemModule : public Krell::IModule {
private:
    mutable std::string cached_data;
    
public:
    SystemModule() = default;
    virtual ~SystemModule() = default;
    
    std::string getName() const override {
        return "System";
    }
    
    void update() override;
    std::string getData() const override;
};

#endif
