#ifndef IMODULE_HPP_
#define IMODULE_HPP_

#include <string>

namespace Krell {

class IModule {
public:
    virtual ~IModule() = default;
    
    virtual std::string getName() const = 0;
    virtual void update() = 0;
    virtual std::string getData() const = 0;
};

}

#endif
