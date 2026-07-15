/*
** EPITECH PROJECT, 2026
** G-PDG-300-NCY-3-1-PDGRUSH4-6
** File description:
** data
*/

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>

#ifndef DATA_HPP_
    #define DATA_HPP_

    /* TOP BAR */
    long swap_avail(void);
    long swap_used(void);
    long swap_free(void);
    long swap_total(void);
    long long mem_bufcache(void);
    long mem_used(void);
    long mem_free(void);
    long mem_total(void);
    double cpu_steal(void);
    double cpu_softirq(void);
    double cpu_irq(void);
    double cpu_iowait(void);
    double cpu_idle(void);
    double cpu_nice(void);
    double cpu_system(void);
    double cpu_user(void);
    int task_zombie(void);
    int task_stopped(void);
    int task_sleeping(void);
    int task_running(void);
    int tasks_total(void);
    int count_user(void);
    std::string my_getloadavg(void);
    std::string time_up(void);
    std::string time_clock(void);

    /* BOTTOM BAR */
    void get_command(const std::string &pid, std::string &command);
    double get_mem_usage(const std::string &pid);
    double get_cpu_usage(const std::string &pid);
    char get_status(const std::string &pid);
    std::string get_res(const std::string &pid);
    std::string get_virt(const std::string &pid);
    int get_ni(const std::string &pid);
    int get_pr(const std::string &pid);
    std::string get_shr(const std::string &pid);
    std::string get_user(const std::string &pid, std::string &user);
    void get_pid(std::vector<std::string> &pid_list);

    /* BATTERY */
    int get_battery_percentage(void);
    std::string get_battery_status(void);
    bool get_ac_connected(void);
    double get_battery_power(void);
    long get_battery_energy_now(void);
    long get_battery_energy_full(void);
    std::string get_battery_time_remaining(void);

    /* SYSTEM INFO */
    std::string get_username(void);
    std::string get_hostname(void);
    std::string get_os(void);
    std::string get_kernel(void);

    /* ERROR HANDLING */
    int error_file(std::ifstream &file);
    int error_dir(DIR *dir);
    int error_handling(std::ifstream &file, DIR *dir);


    /* DISPLAY MODES */
    void launch_ncurses_mode(void);
    int sfml_graphic();
    std::string get_cpu_model();

class Data {
    public:
        Data() = default;
        virtual ~Data() = default;
    protected:
    private:
};

#endif /* !DATA_HPP_ */
