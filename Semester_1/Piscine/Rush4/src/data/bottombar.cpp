/*
** EPITECH PROJECT, 2026
** G-PDG-300-NCY-3-1-PDGRUSH4-6
** File description:
** bottombar
*/

#include "../../include/data.hpp"

void get_command(const std::string &pid, std::string &command)
{
    std::string path = "/proc/" + pid + "/comm";
    std::ifstream file(path);

    if (error_file(file) != 0) {
        command = "?";
        return;
    }
    file >> command;
}

// ! permet de savoir la ram utilise par un process
static long get_rss_from_proc(const std::string &pid)
{
    std::string line;
    std::string label;
    long rss = 0;
    std::string path = "/proc/" + pid + "/status";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return 0;
    while (std::getline(file, line)) {
        if (line.find("VmRSS:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> rss;
            break;
        }
    }
    file.close();
    return rss;
}

static long get_total_memory(void)
{
    std::string line;
    std::string label;
    long memtotal = 0;
    std::ifstream memfile("/proc/meminfo");

    if (error_file(memfile) != 0)
        return 0;
    while (std::getline(memfile, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> memtotal;
            break;
        }
    }
    memfile.close();
    return memtotal;
}

static double calculate_memory_percentage(long rss, long memtotal)
{
    if (memtotal == 0)
        return 0.0;
    return (rss * 100.0) / memtotal;
}

double get_mem_usage(const std::string &pid)
{
    long rss = get_rss_from_proc(pid);
    long memtotal = get_total_memory();
    return calculate_memory_percentage(rss, memtotal);
}

double get_cpu_usage(const std::string &pid)
{
    unsigned long utime = 0;
    unsigned long stime = 0;
    std::string bin;
    std::string path = "/proc/" + pid + "/stat";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return 0.0;
    for (int i = 0; i < 11; ++i)
        file >> bin;
    file >> utime >> stime;
    file.close();

    unsigned long total_time = utime + stime;
    return static_cast<double>(total_time);
}

char get_status(const std::string &pid)
{
    char status = 'S';
    std::string bin;
    std::string path = "/proc/" + pid + "/stat";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return status;
    file >> bin >> bin >> status;
    return status;
}

// ! duplication publique de get_rss_from_proc
std::string get_res(const std::string &pid)
{
    std::string label;
    std::string line;
    long res_kb;
    std::string path = "/proc/" + pid + "/status";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return "0";
    while (std::getline(file, line)) {
        if (line.find("VmRSS:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> res_kb;
            return std::to_string(res_kb);
        }
    }
    return "0";
}

std::string get_virt(const std::string &pid)
{
    std::string label;
    std::string line;
    long virt_kb;
    std::string path = "/proc/" + pid + "/status";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return "0";
    while (std::getline(file, line)) {
        if (line.find("VmSize:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> virt_kb;
            return std::to_string(virt_kb);
        }
    }
    return "0";
}

int get_ni(const std::string &pid)
{
    int nice_value = 0;
    std::string bin;
    std::string path = "/proc/" + pid + "/stat";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return 0;
    for (int i = 0; i < 18; ++i)
        file >> bin;
    file >> nice_value;
    return nice_value;
}

int get_pr(const std::string &pid)
{
    int priority = 0;
    std::string bin;
    std::string path = "/proc/" + pid + "/stat";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return 0;
    for (int i = 0; i < 17; ++i)
        file >> bin;
    file >> priority;
    return priority;
}

// ! shared mem sized
std::string get_shr(const std::string &pid)
{
    std::string line;
    std::string label;
    long shr_kb;
    std::string path = "/proc/" + pid + "/status";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return "0";
    while (std::getline(file, line)) {
        if (line.find("VmLib:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> shr_kb;
            return std::to_string(shr_kb);
        }
    }
    return "0";
}

static int get_uid_from_proc(const std::string &pid)
{
    int uid = -1;
    std::string line;
    std::string label;
    std::string path = "/proc/" + pid + "/status";
    std::ifstream file(path);

    if (error_file(file) != 0)
        return -1;
    while (std::getline(file, line)) {
        if (line.find("Uid:") != std::string::npos) {
            std::istringstream iss(line);
            iss >> label >> uid;
            break;
        }
    }
    file.close();
    return uid;
}

static std::string convert_uid_to_username(int uid)
{
    struct passwd *userinfo = getpwuid(uid);

    if (userinfo == nullptr)
        return std::to_string(uid);
    return userinfo->pw_name;
}

std::string get_user(const std::string &pid, std::string &user)
{
    int uid = get_uid_from_proc(pid);

    if (uid < 0) {
        user = "?";
        return "?";
    }
    user = convert_uid_to_username(uid);
    return user;
}

void get_pid(std::vector<std::string> &pid_list)
{
    struct dirent *entry;
    DIR *dir = opendir("/proc/");

    if (dir == nullptr) {
        return;
    }
    while ((entry = readdir(dir)) != nullptr) {
        if (std::isdigit(entry->d_name[0])) {
            pid_list.push_back(entry->d_name);
        }
    }
    closedir(dir);
}
