#include "BatteryModule.hpp"
#include "data.hpp"

void BatteryModule::update()
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1);
    stream << "Percentage: " << get_battery_percentage() << "% | ";
    stream << "Status: " << get_battery_status() << " | ";
    stream << "Power: " << get_battery_power() << "W | ";
    stream << "Energy: " << get_battery_energy_now() << " / " << get_battery_energy_full() << " | ";
    stream << "Time: " << get_battery_time_remaining();
    
    cached_data = stream.str();
}

std::string BatteryModule::getData() const
{
    return cached_data;
}
