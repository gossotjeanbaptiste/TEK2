#include "../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>

std::string get_cpu_model()
{
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    std::string model_name;

    if (!file.is_open()) {
        return "Unknown";
    }

    while (std::getline(file, line)) {
        if (line.find("model name") != std::string::npos) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                model_name = line.substr(pos + 1);
                size_t start = model_name.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    model_name = model_name.substr(start);
                }
                file.close();
                return model_name;
            }
        }
    }

    file.close();
    return "Unknown";
}