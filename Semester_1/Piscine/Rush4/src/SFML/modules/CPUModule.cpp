#include "CPUModule.hpp"
#include "data.hpp"

void CPUModule::update()
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1);
    stream << "User: " << cpu_user() << "% | ";
    stream << "System: " << cpu_system() << "% | ";
    stream << "Nice: " << cpu_nice() << "% | ";
    stream << "Idle: " << cpu_idle() << "% | ";
    stream << "IOwait: " << cpu_iowait() << "% | ";
    stream << "IRQ: " << cpu_irq() << "% | ";
    stream << "Soft IRQ: " << cpu_softirq() << "% | ";
    stream << "Steal: " << cpu_steal() << "%";
    
    cached_data = stream.str();
}

std::string CPUModule::getData() const
{
    return cached_data;
}
