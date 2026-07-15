/*
** EPITECH PROJECT, 2026
** G-PDG-300-NCY-3-1-PDGRUSH4-6
** File description:
** battery
*/

#include "../../include/data.hpp"

std::string get_battery_path(void)
{
    struct dirent *entry;
    std::string bat_path = "";
    DIR *dir = opendir("/sys/class/power_supply");

    if (error_dir(dir) != 0)
        return "";
    
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name[0] == 'B' && name[1] == 'A' && name[2] == 'T') {
            bat_path = "/sys/class/power_supply/" + name;
            break;
        }
    }
    closedir(dir);
    return bat_path;
}

std::string get_ac_adapter_path(void)
{
    struct dirent *entry;
    std::string ac_path = "";
    DIR *dir = opendir("/sys/class/power_supply");

    if (error_dir(dir) != 0)
        return "";
    
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name[0] == 'A' && name[1] == 'D' && name[2] == 'P') {
            ac_path = "/sys/class/power_supply/" + name;
            break;
        }
    }
    closedir(dir);
    return ac_path;
}

int get_battery_percentage(void)
{
    int percentage = 0;
    std::ifstream file(get_battery_path() + "/capacity");

    if (error_file(file) != 0)
        return 0;
    file >> percentage;
    file.close();
    return percentage;
}

std::string get_battery_status(void)
{
    std::string status = "";
    std::ifstream file(get_battery_path() + "/status");

    if (error_file(file) != 0)
        return "Unknown";
    std::getline(file, status);
    file.close();
    return status;
}

bool get_ac_connected(void)
{
    int online = 0;
    std::ifstream file(get_ac_adapter_path() + "/online");

    if (error_file(file) != 0)
        return false;
    file >> online;
    file.close();
    return (online == 1);
}

double get_battery_power(void)
{
    long power = 0;
    std::string bat_path = get_battery_path();
    std::ifstream power_file(bat_path + "/power_now");

    if (power_file.is_open()) {
        power_file >> power;
        power_file.close();
        return power / 1000000.0;
    }

    std::ifstream current_file(bat_path + "/current_now");
    std::ifstream voltage_file(bat_path + "/voltage_now");
    long current = 0, voltage = 0;

    if (error_file(current_file) == 0 && error_file(voltage_file) == 0) {
        current_file >> current;
        voltage_file >> voltage;
        current_file.close();
        voltage_file.close();
        return (double)(current * voltage) / 1000000.0;
    }
    return 0;
}

long get_battery_energy_now(void)
{
    long energy = 0;
    std::string bat_path = get_battery_path();
    std::ifstream file(bat_path + "/energy_now");

    if (error_file(file) == 0) {
        file >> energy;
        file.close();
        return energy;
    }

    file.open(bat_path + "/charge_now");
    if (error_file(file) == 0) {
        file >> energy;
        file.close();
    }
    return energy;
}

long get_battery_energy_full(void)
{
    long energy = 0;
    std::string bat_path = get_battery_path();
    std::ifstream file(bat_path + "/energy_full");

    if (error_file(file) == 0) {
        file >> energy;
        file.close();
        return energy;
    }

    file.open(bat_path + "/charge_full");
    if (error_file(file) == 0) {
        file >> energy;
        file.close();
    }
    return energy;
}

static std::string format_time(double hours)
{
    int h = (int)hours;
    int m = (int)((hours - h) * 60);
    std::string result = "";

    if (h < 10)
        result += "0";
    result += std::to_string(h) + ":";
    if (m < 10)
        result += "0";
    result += std::to_string(m);
    
    return result;
}

static double calculate_remaining_hours(const std::string &status, double power, long energy_now, long energy_full)
{
    if (status == "Discharging") {
        return (double)energy_now / power / 1000000.0;
    } else if (status == "Charging") {
        if (energy_full > energy_now)
            return (double)(energy_full - energy_now) / power / 1000000.0;
        else
            return 0.0;
    }
    return 0.0;
}

static std::string check_special_cases(const std::string &status, double power, long energy_now)
{
    if (status == "Full")
        return "00:00";
    if (get_ac_connected() && status != "Discharging")
        return "AC";
    if (power == 0 || energy_now == 0)
        return "Unknown";
    return "";
}

std::string get_battery_time_remaining(void)
{
    std::string status = get_battery_status();
    double power = get_battery_power();
    long energy_now = get_battery_energy_now();
    long energy_full = get_battery_energy_full();

    std::string special = check_special_cases(status, power, energy_now);
    if (!special.empty())
        return special;
    double hours = calculate_remaining_hours(status, power, energy_now, energy_full);
    return format_time(hours);
}