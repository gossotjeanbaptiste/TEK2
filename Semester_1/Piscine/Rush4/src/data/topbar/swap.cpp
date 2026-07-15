#include "../../../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
int error_file(std::ifstream &file);

long swap_avail(void)
{
    std::string line;
    std::string label;
    long memavail = 0;
    std::ifstream file("/proc/meminfo");

    error_file(file);
    while (std::getline(file, line)) {
        if (line.find("MemAvailable:") != std::string::npos) {
            std::istringstream iss(line);            
            iss >> label >> memavail;
            break;
        }
    }
    file.close();
    return memavail / 1024;
}

long swap_used(void)
{
    std::string line;
    std::string label;
    long swaptotal = 0;
    long swapfree = 0;
    std::ifstream file("/proc/meminfo");

    error_file(file);
    while (std::getline(file, line)) {
        if (line.find("SwapTotal:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> swaptotal;
        } else if (line.find("SwapFree:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> swapfree;
        }
    }
    file.close();
    return (swaptotal - swapfree) / 1024;
}

long swap_free(void)
{
    std::string line;
    std::string label;
    long swapfree = 0;
    std::ifstream file("/proc/meminfo");

    error_file(file);
    while (std::getline(file, line)) {
        if (line.find("SwapFree:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> swapfree;
            break;
        }
    }
    file.close();
    return swapfree / 1024;
}

long swap_total(void)
{
    std::string line;
    std::string label;
    long swaptotal = 0;
    std::ifstream file("/proc/meminfo");

    error_file(file);
    while (std::getline(file, line)) {
        if (line.find("SwapTotal:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> swaptotal;
            break;
        }
    }
    file.close();
    return swaptotal / 1024;
}
