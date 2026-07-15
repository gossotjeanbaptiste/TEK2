#include "ModuleManager.hpp"

int sfml_graphic()
{
    ModuleManager manager;
    
    manager.setDisplay("SFML");
    
    manager.addModule("CPU");
    manager.addModule("RAM");
    manager.addModule("Battery");
    manager.addModule("System");
    manager.addModule("Tasks");
    
    manager.run();
    
    return 0;
}
