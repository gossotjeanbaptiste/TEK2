#include "ModuleManager.hpp"
#include "CPUModule.hpp"
#include "RAMModule.hpp"
#include "BatteryModule.hpp"
#include "SystemModule.hpp"
#include "TasksModule.hpp"
#include "SFMLDisplay.hpp"
#include "data.hpp"
#include <iostream>
#include <algorithm>

ModuleManager::ModuleManager()
{
    registerModuleFactory("CPU", []() { return std::make_shared<CPUModule>(); });
    registerModuleFactory("RAM", []() { return std::make_shared<RAMModule>(); });
    registerModuleFactory("Battery", []() { return std::make_shared<BatteryModule>(); });
    registerModuleFactory("System", []() { return std::make_shared<SystemModule>(); });
    registerModuleFactory("Tasks", []() { return std::make_shared<TasksModule>(); });
    
    registerDisplayFactory("SFML", []() { return std::make_shared<SFMLDisplay>(); });
}

void ModuleManager::registerModuleFactory(const std::string &name, std::function<std::shared_ptr<Krell::IModule>()> factory)
{
    module_factories[name] = factory;
}

void ModuleManager::registerDisplayFactory(const std::string &name, std::function<std::shared_ptr<Krell::IDisplay>()> factory)
{
    display_factories[name] = factory;
}

void ModuleManager::addModule(const std::string &name)
{
    if (module_factories.find(name) != module_factories.end()) {
        modules.push_back(module_factories[name]());
    }
}

void ModuleManager::removeModule(const std::string &name)
{
    modules.erase(
        std::remove_if(modules.begin(), modules.end(),
            [&name](const std::shared_ptr<Krell::IModule> &module) {
                return module->getName() == name;
            }
        ),
        modules.end()
    );
}

void ModuleManager::setDisplay(const std::string &name)
{
    if (display_factories.find(name) != display_factories.end()) {
        current_display = display_factories[name]();
        current_display->init();
    }
}

void ModuleManager::run_frame()
{
    if (current_display && current_display->isRunning()) {
        current_display->render(modules);
    }
}

void ModuleManager::run()
{
    while (current_display && current_display->isRunning()) {
        run_frame();
        
        if (auto sfml_display = std::dynamic_pointer_cast<SFMLDisplay>(current_display)) {
            if (sfml_display->shouldSwitchToNcurses()) {
                current_display->shutdown();
                launch_ncurses_mode();
                exit(0);
            }
        }
    }
}

std::vector<std::shared_ptr<Krell::IModule>> &ModuleManager::getModules()
{
    return modules;
}

std::shared_ptr<Krell::IDisplay> ModuleManager::getDisplay() const
{
    return current_display;
}
