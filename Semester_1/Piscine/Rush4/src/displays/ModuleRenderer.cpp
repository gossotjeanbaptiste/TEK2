#include "SFMLDisplay.hpp"

static std::vector<std::string> split_module_data(const std::string &data)
{
    std::vector<std::string> parts;
    size_t pos = 0;
    while (pos < data.length()) {
        size_t next = data.find(" | ", pos);
        if (next == std::string::npos) next = data.length();
        parts.push_back(data.substr(pos, next - pos));
        pos = next + 3;
    }
    return parts;
}

static sf::Color get_module_color(const std::string &module_name)
{
    if (module_name == "CPU") return sf::Color::Yellow;
    else if (module_name == "RAM") return sf::Color::Cyan;
    else if (module_name == "Battery") return sf::Color::Magenta;
    else if (module_name == "System") return sf::Color::Red;
    return sf::Color::White;
}

void SFMLDisplay::draw_module_data(const std::vector<std::string> &parts, sf::Color color, int &y_pos)
{
    int line_y = y_pos + 35;
    size_t per_line = (parts.size() + 1) / 2;
    for (size_t i = 0; i < parts.size(); i += per_line) {
        std::string line = "";
        for (size_t j = i; j < i + per_line && j < parts.size(); ++j) {
            if (j > i) line += " | ";
            line += parts[j];
        }
        sf::Text text(line, font, 16);
        text.setPosition(40, line_y);
        text.setFillColor(color);
        window.draw(text);
        line_y += 25;
    }
    y_pos += 120;
}

void SFMLDisplay::render_modules(const std::vector<std::shared_ptr<Krell::IModule>> &modules)
{
    if (!window.isOpen()) {
        running = false;
        return;
    }
    int y_pos = 60;
    for (const auto &module : modules) {
        if (!window.isOpen()) {
            running = false;
            return;
        }
        if (module->getName() == "Tasks")
            continue;
        sf::Text module_title(module->getName(), font, 24);
        module_title.setPosition(20, y_pos);
        module_title.setFillColor(sf::Color::White);
        window.draw(module_title);
        std::vector<std::string> parts = split_module_data(module->getData());
        sf::Color color = get_module_color(module->getName());
        draw_module_data(parts, color, y_pos);
    }
    if (!window.isOpen()) {
        running = false;
        return;
    }
    auto tasks_module = std::find_if(modules.begin(), modules.end(),
        [](const std::shared_ptr<Krell::IModule> &m) { return m->getName() == "Tasks"; });
    if (tasks_module != modules.end()) {
        render_tasks_menu(*tasks_module);
    }
}
