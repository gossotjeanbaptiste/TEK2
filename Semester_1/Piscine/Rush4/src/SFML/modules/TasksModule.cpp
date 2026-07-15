#include "TasksModule.hpp"
#include "data.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>

std::string get_username_from_uid(int uid)
{
    std::ifstream passwd_file("/etc/passwd");
    std::string line;
    while (std::getline(passwd_file, line)) {
        std::istringstream iss(line);
        std::string username, x;
        int file_uid;
        std::getline(iss, username, ':');
        std::getline(iss, x, ':');
        iss >> file_uid;
        if (file_uid == uid) {
            return username;
        }
    }
    return std::to_string(uid);
}

void TasksModule::update()
{
    std::vector<std::string> pid_list;
    get_pid(pid_list);
    
    tasks_data.clear();
    
    for (const auto &pid : pid_list) {
        std::string user = pid;
        std::string command;
        int pr = 0;
        int ni = 0;
        std::string virt = "0";
        std::string res = "0";
        std::string shr = "0";
        double cpu_percent = 0.0;
        double mem_percent = 0.0;
        
        try {
            std::string path = "/proc/" + pid + "/status";
            std::ifstream file(path);
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    if (line.find("Uid:") != std::string::npos) {
                        std::istringstream iss(line);
                        std::string label;
                        int uid;
                        iss >> label >> uid;
                        user = get_username_from_uid(uid);
                        break;
                    }
                }
                file.close();
            }
        } catch (...) {}
        
        try {
            pr = get_pr(pid);
            ni = get_ni(pid);
            virt = get_virt(pid);
            res = get_res(pid);
            shr = get_shr(pid);
            get_command(pid, command);
            cpu_percent = get_cpu_usage(pid);
            mem_percent = get_mem_usage(pid);
        } catch (...) {
            continue;
        }
        
        if (command.length() > 10)
            command = command.substr(0, 7) + "...";
        
        std::ostringstream task_stream;
        task_stream << std::left << std::setw(6) << pid 
                    << std::left << std::setw(6) << user 
                    << std::left << std::setw(4) << pr
                    << std::left << std::setw(4) << ni
                    << std::left << std::setw(6) << virt
                    << std::left << std::setw(6) << res
                    << std::left << std::setw(6) << shr
                    << std::fixed << std::setprecision(1) << std::setw(5) << cpu_percent
                    << std::fixed << std::setprecision(1) << std::setw(5) << mem_percent
                    << std::left << command;
        
        tasks_data.push_back(task_stream.str());
    }
    
    std::ostringstream summary;
    summary << "Total: " << pid_list.size() << " processes";
    cached_data = summary.str();
}

std::string TasksModule::getData() const
{
    return cached_data;
}

std::vector<std::string> TasksModule::getTasksData() const
{
    return tasks_data;
}
