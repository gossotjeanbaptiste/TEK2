#include <ncurses.h>
#include <vector>
#include <string>
#include "../include/data.hpp"
#include "../include/sfml.hpp"

void launch_ncurses_mode() {
    initscr();
    noecho();
    curs_set(0);
    timeout(1000);
    keypad(stdscr, TRUE);

    int scroll_offset = 0;

    while (true) {
        int ch = getch();
        if (ch == 'q') break;
        if (ch == 's') {
            sfml_graphic();
            endwin();
            return;
        }

        if (ch == KEY_DOWN) {
            scroll_offset++;
        } else if (ch == KEY_UP) {
            if (scroll_offset > 0) {
                scroll_offset--;
            }
        } else if (ch == KEY_NPAGE) {
            scroll_offset += 10;
        } else if (ch == KEY_PPAGE) {
            scroll_offset -= 10;
            if (scroll_offset < 0) {
                scroll_offset = 0;
            }
        }
        
        erase();
        
        mvprintw(0, 0, "%s, up %s, %d user(s), load average: %s, battery: %d %% (%s remaining)", 
        time_clock().c_str(), time_up().c_str(), count_user(), my_getloadavg().c_str(), get_battery_percentage(), get_battery_time_remaining().c_str());
        mvprintw(1, 0, "System Information: Username: %s, Hostname: %s, Operating System: %s, Kernel: %s",
            get_username().c_str(), get_hostname().c_str(), get_os().c_str(), get_kernel().c_str());
        mvprintw(2, 0, "Tasks: %d total, %d running, %d sleeping, %d stopped, %d zombie",
                 tasks_total(), task_running(), task_sleeping(), task_stopped(), task_zombie());
        mvprintw(3, 0, "%%Cpu(s): %.1f us, %.1f sy, %.1f ni, %.1f id, %.1f wa, %.1f hi, %.1f si, %.1f st",
                 cpu_user(), cpu_system(), cpu_nice(), cpu_idle(), cpu_iowait(), cpu_irq(), cpu_softirq(), cpu_steal());
        mvprintw(4, 0, "CPU Model : %s", get_cpu_model().c_str());
        mvprintw(5, 0, "MiB Mem : %ld total, %ld free, %ld used, %lld buff/cache",
                 mem_total(), mem_free(), mem_used(), mem_bufcache());
        mvprintw(6, 0, "MiB Swap: %ld total, %ld free, %ld used, %ld avail Mem",
                 swap_total(), swap_free(), swap_used(), swap_avail());
        mvprintw(7, 0, "");
        attron(A_REVERSE);
        mvprintw(8, 0, "%-7s\t%-10s\t%-3s\t%-3s\t%-7s\t%-7s\t%-7s\t%-2s\t%-5s\t%-5s\t%-10s", 
                 "PID", "USER", "PR", "NI", "VIRT", "RES", "SHR", "S", "%CPU", "%MEM", "COMMAND");
        attroff(A_REVERSE);

        std::vector<std::string> pids;
        get_pid(pids);

        if (scroll_offset >= (int)pids.size()) {
            scroll_offset = pids.size() - 1;
        }
        if (scroll_offset < 0) {
            scroll_offset = 0;
        }
        
        int line = 9;
        int max_lines = LINES - 2;
        for (int i = scroll_offset; i < (int)pids.size() && line < max_lines; ++i) {
            std::string user, cmd;
            get_user(pids[i], user);
            get_command(pids[i], cmd);
            mvprintw(line++, 0, "%-7s\t%-10s\t%-3d\t%-3d\t%-7s\t%-7s\t%-7s\t%-2c\t%-5.1f\t%-5.1f\t%-10s",
                     pids[i].c_str(), 
                     user.c_str(), 
                     get_pr(pids[i]), 
                     get_ni(pids[i]),
                     get_virt(pids[i]).c_str(),
                     get_res(pids[i]).c_str(),
                     get_shr(pids[i]).c_str(),
                     get_status(pids[i]),
                     get_cpu_usage(pids[i]),
                     get_mem_usage(pids[i]),
                     cmd.c_str());
        }

        attron(A_REVERSE);
        mvprintw(LINES - 1, 0, "Tasks: %zu | Showing %d-%d | q:quit, s:SFML, UP/DOWN to scroll (1), PageUp/PageDown for faster scroll (10)", 
                 pids.size(), scroll_offset + 1, (scroll_offset + max_lines - 9 > (int)pids.size()) ? (int)pids.size() : scroll_offset + max_lines - 9);
        attroff(A_REVERSE);

        refresh();

    }
    endwin();
}