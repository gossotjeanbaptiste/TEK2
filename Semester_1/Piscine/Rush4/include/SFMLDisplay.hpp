#ifndef SFMLDISPLAY_HPP_
#define SFMLDISPLAY_HPP_

#include "IDisplay.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class SFMLDisplay : public Krell::IDisplay {
private:
    sf::RenderWindow window;
    sf::Font font;
    int scroll_offset;
    bool running;
    bool switch_to_ncurses;
    
    void load_font();
    void setup_window();
    void handle_events();
    void handle_close_event();
    void handle_scroll_event(const sf::Event &event);
    void handle_key_event(const sf::Event &event);
    void render_modules(const std::vector<std::shared_ptr<Krell::IModule>> &modules);
    void draw_module_data(const std::vector<std::string> &parts, sf::Color color, int &y_pos);
    void render_tasks_menu(const std::shared_ptr<Krell::IModule> &tasks_module);
    void draw_tasks_panel(int x_pos, int y_pos, int width, int height);
    void draw_tasks_title(int x_pos, int y_pos);
    void draw_tasks_list(const std::vector<std::string> &tasks_data, int x_pos, int y_pos, int height);
    void draw_tasks_count(const std::string &count_text, int x_pos, int y_pos, int height);
    void draw_background();
    void draw_title();
    void update_modules(const std::vector<std::shared_ptr<Krell::IModule>> &modules);
    void display_window();
    void display_cpu_module_v2(const std::shared_ptr<Krell::IModule> &module);
    void display_ram_module_v2(const std::shared_ptr<Krell::IModule> &module);
    void display_battery_module_v2(const std::shared_ptr<Krell::IModule> &module);
    
public:
    SFMLDisplay();
    virtual ~SFMLDisplay();
    
    std::string getName() const override;
    void init() override;
    void render(const std::vector<std::shared_ptr<Krell::IModule>> &modules) override;
    void shutdown() override;
    bool isRunning() const override;
    bool shouldSwitchToNcurses() const;
};

#endif
