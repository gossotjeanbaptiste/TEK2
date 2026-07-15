#include "SFMLDisplay.hpp"

void SFMLDisplay::draw_background()
{
    sf::Color gris(50, 50, 50);
    window.clear(gris);
}

void SFMLDisplay::draw_title()
{
    sf::Text title("MyGKrellm - System Monitor", font, 24);
    title.setPosition(50, 10);
    title.setFillColor(sf::Color::White);
    window.draw(title);
}

void SFMLDisplay::update_modules(const std::vector<std::shared_ptr<Krell::IModule>> &modules)
{
    for (auto &module : modules) {
        if (!window.isOpen()) {
            running = false;
            return;
        }
        const_cast<Krell::IModule*>(module.get())->update();
    }
}

void SFMLDisplay::display_window()
{
    if (window.isOpen()) {
        window.display();
    } else {
        running = false;
    }
}

void SFMLDisplay::render(const std::vector<std::shared_ptr<Krell::IModule>> &modules)
{
    if (!window.isOpen()) {
        running = false;
        return;
    }
    
    handle_events();
    
    if (!window.isOpen()) {
        running = false;
        return;
    }
    
    try {
        draw_background();
        draw_title();
        update_modules(modules);
        render_modules(modules);
        display_window();
    } catch (...) {
        running = false;
        window.close();
    }
}
