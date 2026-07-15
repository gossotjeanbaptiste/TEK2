#ifndef IDISPLAY_HPP_
#define IDISPLAY_HPP_

#include <memory>
#include <vector>
#include "IModule.hpp"

namespace Krell {

class IDisplay {
public:
    virtual ~IDisplay() = default;
    
    virtual std::string getName() const = 0;
    virtual void init() = 0;
    virtual void render(const std::vector<std::shared_ptr<IModule>> &modules) = 0;
    virtual void shutdown() = 0;
    virtual bool isRunning() const = 0;
};

}

#endif
