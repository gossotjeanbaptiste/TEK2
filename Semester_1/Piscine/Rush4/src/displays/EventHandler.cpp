#include "SFMLDisplay.hpp"

void SFMLDisplay::handle_close_event()
{
    window.close();
    running = false;
}

void SFMLDisplay::handle_scroll_event(const sf::Event &event)
{
    if (event.mouseWheelScroll.delta > 0) {
        scroll_offset -= 25;
    } else {
        scroll_offset += 25;
    }
}

void SFMLDisplay::handle_key_event(const sf::Event &event)
{
    if (event.key.code == sf::Keyboard::Q) {
        handle_close_event();
        return;
    }
    if (event.key.code == sf::Keyboard::S) {
        window.close();
        running = false;
        switch_to_ncurses = true;
    }
}

void SFMLDisplay::handle_events()
{
    if (!window.isOpen()) return;
    
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            handle_close_event();
            return;
        }
        if (event.type == sf::Event::MouseWheelScrolled) {
            handle_scroll_event(event);
        }
        if (event.type == sf::Event::KeyPressed) {
            handle_key_event(event);
        }
    }
}
