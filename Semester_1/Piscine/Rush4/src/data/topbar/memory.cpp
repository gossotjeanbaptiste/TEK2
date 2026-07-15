#include "../../../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
int error_file(std::ifstream &file);


long long mem_bufcache(void)
{
    std::string line, label;
    long long value;
    long long buffers = 0;
    long long cached = 0; 
    long long sreclaimable = 0;
    std::ifstream file("/proc/meminfo");

    if (!file) return 0;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        if (!(iss >> label >> value)) 
            continue;
        if (label == "Buffers:")
            buffers = value;
        else if (label == "Cached:")
            cached = value;
        else if (label == "SReclaimable:")
            sreclaimable = value;
    }
    file.close();
    return (buffers + cached + sreclaimable) / 1024;
}

long mem_used(void)
{
    std::string line;
    std::string label;
    long memtotal = 0;
    long memavailable = 0;
    std::ifstream file("/proc/meminfo");

    error_file(file);
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> memtotal;
        } else if (line.find("MemAvailable:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> memavailable;
        }
    }
    file.close();
    return (memtotal - memavailable) / 1024;
}

long mem_free(void)
{
    std::string line;
    std::string label;
    long memavailable = 0;
    std::ifstream file("/proc/meminfo");

    error_file(file);
    while (std::getline(file, line)) {
        if (line.find("MemAvailable:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> memavailable;
            break;
        }
    }
    file.close();
    return memavailable / 1024;
}

long mem_total(void)
{
    std::string line;
    std::string label;
    long memtotal = 0;
    std::ifstream file("/proc/meminfo");

    error_file(file);
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> memtotal;
            break;
        }
    }
    file.close();
    return memtotal / 1024;
}
