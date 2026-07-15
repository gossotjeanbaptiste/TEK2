/*
** EPITECH PROJECT, 2026
** G-PDG-300-NCY-3-1-PDGRUSH4-6
** File description:
** systeminfo
*/

#include "../../include/data.hpp"

std::string get_username(void)
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);

    if (pw == nullptr)
        return "unknown";
    return std::string(pw->pw_name);
}

std::string get_hostname(void)
{
    std::string hostname;
    std::ifstream file("/proc/sys/kernel/hostname");

    if (error_file(file) != 0)
        return "unknown";
    if (!std::getline(file, hostname))
        return "unknown";
    if (!hostname.empty() && hostname.back() == '\n')
        hostname.pop_back();
    return hostname;
}

std::string get_os(void)
{
    std::string line;
    std::string pretty_name = "unknown";
    std::ifstream file("/etc/os-release");

    if (error_file(file) != 0)
        return pretty_name;
    while (std::getline(file, line)) {
        if (line.find("PRETTY_NAME=") == 0) {
            size_t start = line.find('"');
            size_t end = line.rfind('"');
            if (start != std::string::npos && 
                end != std::string::npos && start != end)
                pretty_name = line.substr(start + 1, end - start - 1);
            break;
        }
    }
    return pretty_name;
}

std::string get_kernel(void)
{
    std::string kernel;
    std::ifstream file("/proc/sys/kernel/osrelease");

    if (error_file(file) != 0)
        return "unknown";
    if (!std::getline(file, kernel))
        return "unknown";
    if (!kernel.empty() && kernel.back() == '\n')
        kernel.pop_back();
    return kernel;
}
