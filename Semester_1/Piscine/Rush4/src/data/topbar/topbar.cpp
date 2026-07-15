#include "../../../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
int error_file(std::ifstream &file);


std::string my_getloadavg(void)
{
    double loadavg[3];
    std::ifstream file("/proc/loadavg");
    error_file(file);
    file >> loadavg[0] >> loadavg[1] >> loadavg[2];
    std::string load1 = std::to_string(loadavg[0]).substr(0, 4);
    std::string load2 = std::to_string(loadavg[1]).substr(0, 4);
    std::string load3 = std::to_string(loadavg[2]).substr(0, 4);
    std::string result = load1 + ", " + load2 + ", " + load3;

    return result;
}
