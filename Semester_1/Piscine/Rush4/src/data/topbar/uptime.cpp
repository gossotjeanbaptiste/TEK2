#include "../../../include/data.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
int error_file(std::ifstream &file);

std::string time_up(void)
{
    double timeup[3];
    std::ifstream file("/proc/uptime");
    error_file(file);
    file >> timeup[0] >> timeup[1];
    int hours = static_cast<int>(timeup[0]) / 3600;
    int minutes = (static_cast<int>(timeup[0]) % 3600) / 60;
    int seconds = static_cast<int>(timeup[0]) % 60;
    std::string min_str = std::to_string(minutes);
    std::string sec_str = std::to_string(seconds);

    if (min_str.length() == 1)
        min_str = "0" + min_str;
    if (sec_str.length() == 1)
        sec_str = "0" + sec_str;
    std::string result = std::to_string(hours) + ":" + min_str + ":" + sec_str;
    return result;
}

std::string time_clock(void)
{
    time_t t = time(nullptr);
    struct tm *currentTime = localtime(&t);
    std::string day = std::to_string(currentTime->tm_mday);
    std::string month = std::to_string(currentTime->tm_mon + 1);
    std::string year = std::to_string(currentTime->tm_year + 1900);
    std::string hour = std::to_string(currentTime->tm_hour);
    std::string min = std::to_string(currentTime->tm_min);
    std::string sec = std::to_string(currentTime->tm_sec);

    if (day.length() == 1)
        day = "0" + day;
    if (month.length() == 1)
        month = "0" + month;
    if (hour.length() == 1)
        hour = "0" + hour;
    if (min.length() == 1)
        min = "0" + min;
    if (sec.length() == 1)
        sec = "0" + sec;
    std::string result = day + "/" + month + "/" + year + ", " + hour + ":" + min + ":" + sec;
    return result;
}
