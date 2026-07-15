#include "SystemModule.hpp"
#include "data.hpp"

void SystemModule::update()
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1);
    stream << "User: " << get_username() << " | ";
    stream << "Hostname: " << get_hostname() << " | ";
    stream << "Operating System: " << get_os() << " | ";
    stream << "Kernel: " << get_kernel();
    
    cached_data = stream.str();
}

std::string SystemModule::getData() const
{
    return cached_data;
}
