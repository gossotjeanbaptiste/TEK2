#include "../../../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
int error_file(std::ifstream &file);

double cpu_steal(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (steal * 100.0) / total;
}

double cpu_softirq(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (softirq * 100.0) / total;
}

double cpu_irq(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (irq * 100.0) / total;
}

double cpu_iowait(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (iowait * 100.0) / total;
}

double cpu_idle(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (idle * 100.0) / total;
}

double cpu_nice(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (nice * 100.0) / total;
}

double cpu_system(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (system * 100.0) / total;
}

double cpu_user(void)
{
    std::string line;
    std::string label;
    long user = 0;
    long nice = 0;
    long system = 0;
    long idle = 0;
    long iowait = 0;
    long irq = 0;
    long softirq = 0;
    long steal = 0;
    std::ifstream file("/proc/stat");

    error_file(file);
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    file.close();

    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    if (total == 0)
        total = 1;

    return (user * 100.0) / total;
}
