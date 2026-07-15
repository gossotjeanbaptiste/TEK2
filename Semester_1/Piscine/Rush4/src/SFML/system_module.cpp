/*
** EPITECH PROJECT, 2026
** G-PDG-300-NCY-3-1-PDGRUSH4-6
** File description:
** system_module
*/

#include <SFML/Graphics.hpp>
#include "../../include/data.hpp"
#include <sstream>
#include <iomanip>

void display_system_title(sf::RenderWindow &window, sf::Font &font, int y_pos)
{
    sf::Text title("System Information", font, 24);
    title.setPosition(20, y_pos);
    title.setFillColor(sf::Color::White);
    window.draw(title);
}

void display_system_line1(sf::RenderWindow &window, sf::Font &font, int y_pos)
{
    std::string username = get_username();
    std::string hostname = get_hostname();

    std::ostringstream line1_stream;
    line1_stream << "Username: " << username << " | Hostname: " << hostname;

    sf::Text line1_text(line1_stream.str(), font, 16);
    line1_text.setPosition(40, y_pos + 35);
    line1_text.setFillColor(sf::Color::Red);
    window.draw(line1_text);
}

void display_system_line2(sf::RenderWindow &window, sf::Font &font, int y_pos)
{
    std::string os = get_os();
    std::string kernel = get_kernel();

    std::ostringstream line2_stream;
    line2_stream << "OperatingSystem: " << os << " | Kernel: " << kernel;

    sf::Text line2_text(line2_stream.str(), font, 16);
    line2_text.setPosition(40, y_pos + 60);
    line2_text.setFillColor(sf::Color::Red);
    window.draw(line2_text);
}

void display_system_module(sf::RenderWindow &window, sf::Font &font, int y_pos)
{
    display_system_title(window, font, y_pos);
    display_system_line1(window, font, y_pos);
    display_system_line2(window, font, y_pos);
}
