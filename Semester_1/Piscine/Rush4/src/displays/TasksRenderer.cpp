#include "SFMLDisplay.hpp"
#include "TasksModule.hpp"

void SFMLDisplay::draw_tasks_panel(int x_pos, int y_pos, int width, int height)
{
    sf::RectangleShape panel(sf::Vector2f(width, height));
    panel.setPosition(x_pos, y_pos);
    panel.setFillColor(sf::Color(40, 40, 40));
    window.draw(panel);

    sf::RectangleShape border(sf::Vector2f(width, height));
    border.setPosition(x_pos, y_pos);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(2);
    window.draw(border);
}

void SFMLDisplay::draw_tasks_title(int x_pos, int y_pos)
{
    sf::Text title("PID USER PR NI VIRT RES SHR %CPU %MEM CMD", font, 18);
    title.setPosition(x_pos + 5, y_pos + 10);
    title.setFillColor(sf::Color::White);
    window.draw(title);
}

void SFMLDisplay::draw_tasks_list(const std::vector<std::string> &tasks_data, int x_pos, int y_pos, int height)
{
    int y_offset = y_pos + 30 - scroll_offset;
    int content_area_top = y_pos + 30;
    int content_area_bottom = y_pos + height - 15;

    for (const auto &task_line : tasks_data) {
        sf::Text task_text(task_line, font, 16);
        task_text.setPosition(x_pos + 5, y_offset);
        task_text.setFillColor(sf::Color::Cyan);
        
        if (y_offset >= content_area_top && y_offset <= content_area_bottom) {
            window.draw(task_text);
        }
        y_offset += 28;
    }
}

void SFMLDisplay::draw_tasks_count(const std::string &count_text_str, int x_pos, int y_pos, int height)
{
    sf::Text count_text(count_text_str, font, 16);
    count_text.setPosition(x_pos + 5, y_pos + height + 10);
    count_text.setFillColor(sf::Color::Yellow);
    window.draw(count_text);
}

void SFMLDisplay::render_tasks_menu(const std::shared_ptr<Krell::IModule> &tasks_module)
{
    if (!window.isOpen()) return;
    
    auto tasks_ptr = std::dynamic_pointer_cast<TasksModule>(tasks_module);
    if (!tasks_ptr)
        return;
    
    int x_pos = 950;
    int y_pos = 50;
    int width = 730;
    int height = 940;
    
    draw_tasks_panel(x_pos, y_pos, width, height);
    draw_tasks_title(x_pos, y_pos);
    draw_tasks_list(tasks_ptr->getTasksData(), x_pos, y_pos, height);
    draw_tasks_count(tasks_module->getData(), x_pos, y_pos, height);
}
