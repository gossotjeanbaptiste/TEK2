
#ifndef SFML_HPP_
#define SFML_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

int sfml_graphic(void);

void setup_window(sf::RenderWindow &window);
void load_font(sf::Font &font);
void handle_events(sf::RenderWindow &window, int &scroll_offset, bool &quit_flag, bool &switch_to_ncurses);
void render_display(sf::RenderWindow &window, sf::Font &font, int scroll_offset);
void run_event_loop(sf::RenderWindow &window, sf::Font &font, int &scroll_offset, bool &quit_flag, bool &switch_to_ncurses);

#endif

