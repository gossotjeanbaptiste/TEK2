#include "SFMLDisplay.hpp"
#include <iostream>

SFMLDisplay::SFMLDisplay() 
    : window(sf::VideoMode(2200, 1050), "MyGKrellm - System Monitor"),
      scroll_offset(0), running(true), switch_to_ncurses(false)
{
}

SFMLDisplay::~SFMLDisplay()
{
    shutdown();
}

std::string SFMLDisplay::getName() const
{
    return "SFML";
}

void SFMLDisplay::setup_window()
{
    window.setFramerateLimit(60);
}

void SFMLDisplay::load_font()
{
    std::vector<std::string> font_paths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/UbuntuMono-R.ttf",
        "./resources/font.ttf"
    };
    
    for (const auto &path : font_paths) {
        if (font.loadFromFile(path)) {
            break;
        }
    }
}

void SFMLDisplay::init()
{
    setup_window();
    load_font();
}

void SFMLDisplay::shutdown()
{
    if (window.isOpen()) {
        window.close();
    }
}

bool SFMLDisplay::isRunning() const
{
    return running && window.isOpen();
}

bool SFMLDisplay::shouldSwitchToNcurses() const
{
    return switch_to_ncurses;
}
