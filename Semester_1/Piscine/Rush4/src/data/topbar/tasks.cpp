#include "../../../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>


int task_zombie(void)
{
    std::string line;
    std::string label;
    int zombie = 0;
    char status;
    std::ifstream file;

    try {
        for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
            if (!std::filesystem::is_directory(entry))
                continue;
            std::string filename = entry.path().filename().string();
            if (filename.empty() || !std::isdigit(filename[0]))
                continue;
            line = "/proc/" + filename + "/stat";
            file.open(line);
            if (file.is_open()) {
                std::string bin;
                if (file >> bin >> bin >> status)
                    if (status == 'Z')
                        zombie++;
                file.close();
            }
        }
    } catch (const std::exception& e) {
        return 0;
    }
    return zombie;
}

int task_stopped(void)
{
    std::string line;
    std::string label;
    int stopped = 0;
    char status;
    std::ifstream file;

    try {
        for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
            if (!std::filesystem::is_directory(entry))
                continue;
            std::string filename = entry.path().filename().string();
            if (filename.empty() || !std::isdigit(filename[0]))
                continue;
            line = "/proc/" + filename + "/stat";
            file.open(line);
            if (file.is_open()) {
                std::string bin;
                if (file >> bin >> bin >> status)
                    if (status == 'T')
                        stopped++;
                file.close();
            }
        }
    } catch (const std::exception& e) {
        return 0;
    }
    return stopped;
}

int task_sleeping(void)
{
    std::string line;
    std::string label;
    int sleeping = 0;
    char status;
    std::ifstream file;

    try {
        for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
            if (!std::filesystem::is_directory(entry))
                continue;
            std::string filename = entry.path().filename().string();
            if (filename.empty() || !std::isdigit(filename[0]))
                continue;
            line = "/proc/" + filename + "/stat";
            file.open(line);
            if (file.is_open()) {
                std::string bin;
                if (file >> bin >> bin >> status)
                    if (status == 'S' || status == 'P' || status == 'D' || status == 'I')
                        sleeping++;
                file.close();
            }
        }
    } catch (const std::exception& e) {
        return 0;
    }
    return sleeping;
}

int task_running(void)
{
    std::string line;
    std::string label;
    int running = 0;
    char status;
    std::ifstream file;

    try {
        for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
            if (!std::filesystem::is_directory(entry))
                continue;
            std::string filename = entry.path().filename().string();
            if (filename.empty() || !std::isdigit(filename[0]))
                continue;
            line = "/proc/" + filename + "/stat";
            file.open(line);
            if (file.is_open()) {
                std::string bin;
                if (file >> bin >> bin >> status)
                    if (status == 'R')
                        running++;
                file.close();
            }
        }
    } catch (const std::exception& e) {
        return 0;
    }
    return running;
}

int tasks_total(void)
{
    int count = 0;

    try {
        for (const auto& entry : std::filesystem::directory_iterator("/proc")) {
            if (!std::filesystem::is_directory(entry))
                continue;
            std::string filename = entry.path().filename().string();
            if (!filename.empty() && std::isdigit(filename[0]))
                count++;
        }
    } catch (const std::exception& e) {
        return 0;
    }
    return count;
}