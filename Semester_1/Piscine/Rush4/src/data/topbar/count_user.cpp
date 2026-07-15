#include "../../../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
int error_file(std::ifstream &file);

int count_user(void)
{
    int count = 0;
    std::string line;
    std::ifstream file("/etc/passwd");

    error_file(file);
    while (std::getline(file, line)) {
        bool has_home = (line.find("/home/") != std::string::npos);
        bool has_bash = (line.find(":/bin/bash") != std::string::npos);
        bool has_sh = (line.find(":/bin/sh") != std::string::npos);
        bool has_zsh = (line.find(":/bin/zsh") != std::string::npos);

        if (has_home && (has_bash || has_sh || has_zsh)) {
            count++;
        }
    }
    file.close();
    return count;
}
