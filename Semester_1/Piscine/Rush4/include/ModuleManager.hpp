#ifndef MODULEMANAGER_HPP_
#define MODULEMANAGER_HPP_

#include "IModule.hpp"
#include "IDisplay.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>

class ModuleManager {
private:
    std::vector<std::shared_ptr<Krell::IModule>> modules;
    std::shared_ptr<Krell::IDisplay> current_display;
    std::map<std::string, std::function<std::shared_ptr<Krell::IModule>()>> module_factories;
    std::map<std::string, std::function<std::shared_ptr<Krell::IDisplay>()>> display_factories;
    
public:
    ModuleManager();
    
    void registerModuleFactory(const std::string &name, std::function<std::shared_ptr<Krell::IModule>()> factory);
    void registerDisplayFactory(const std::string &name, std::function<std::shared_ptr<Krell::IDisplay>()> factory);
    
    void addModule(const std::string &name);
    void removeModule(const std::string &name);
    void setDisplay(const std::string &name);
    
    void run();
    void run_frame();
    
    std::vector<std::shared_ptr<Krell::IModule>> &getModules();
    std::shared_ptr<Krell::IDisplay> getDisplay() const;
};

#endif
