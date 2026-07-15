#include "RAMModule.hpp"
#include "data.hpp"

void RAMModule::update()
{
    std::ostringstream stream;
    stream << "Total: " << mem_total() << " | ";
    stream << "Used: " << mem_used() << " | ";
    stream << "Free: " << mem_free() << " | ";
    stream << "Buff/Cache: " << mem_bufcache() << " | ";
    stream << "Swap Total: " << swap_total() << " | ";
    stream << "Swap Used: " << swap_used();
    
    cached_data = stream.str();
}

std::string RAMModule::getData() const
{
    return cached_data;
}
