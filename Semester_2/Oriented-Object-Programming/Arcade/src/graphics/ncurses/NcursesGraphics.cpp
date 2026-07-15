/*
 ** EPITECH PROJECT, 2024
 ** Arcade
 ** File description:
 ** NcursesGraphics implementation
 */

#include "NcursesGraphics.hpp"
#include "IEvents.hpp"
#include "KeyCodes.hpp"
#include <thread>
#include <unistd.h>


arc::NcursesGraphics::NcursesGraphics() : _width(80), _height(24), _initialized(false)
{
}

void arc::NcursesGraphics::init()
{
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    timeout(0);
    set_escdelay(0);
    keypad(stdscr, TRUE);
    curs_set(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    getmaxyx(stdscr, _height, _width);
    _initialized = true;
}

void arc::NcursesGraphics::close()
{
    if (_initialized) {
        endwin();
        _initialized = false;
    }
}

void arc::NcursesGraphics::clear()
{
    erase();
}

void arc::NcursesGraphics::drawChar(int x, int y, char character, int color)
{
    (void)color;
    if (y >= 0 && y < _height && x >= 0 && x < _width) {
        mvaddch(y, x, character);
    }
}

void arc::NcursesGraphics::drawString(int x, int y, const std::string& text, int color)
{
    (void)color;
    if (y >= 0 && y < _height && x >= 0 && x < _width) {
        mvaddstr(y, x, text.c_str());
    }
}

void arc::NcursesGraphics::refresh()
{
    ::refresh();
    usleep(16000);
}

void arc::NcursesGraphics::_processMouseEvent(arc::Event& event)
{
    MEVENT mouseEvent;
    if (getmouse(&mouseEvent) != OK)
        return;

    event.mouseX = mouseEvent.x;
    event.mouseY = mouseEvent.y;

    if (mouseEvent.bstate & BUTTON1_CLICKED) {
        event.type = EventType::MousePressed;
        event.key = 1;  // Left click
    } else if (mouseEvent.bstate & BUTTON3_CLICKED) {
        event.type = EventType::MousePressed;
        event.key = 3;  // Right click
    } else if (mouseEvent.bstate & BUTTON2_CLICKED) {
        event.type = EventType::MousePressed;
        event.key = 2;  // Middle click
    } else {
        event.type = EventType::MouseMoved;
    }
}

void arc::NcursesGraphics::_processKeyboardEvent(int ch, arc::Event& event)
{
    if (ch == 27) {
        event.type = EventType::Quit;
    } else if (ch == KEY_MOUSE) {
        _processMouseEvent(event);
    } else {
        event.type = EventType::KeyPressed;
        event.key = ch;
    }
}

arc::Event arc::NcursesGraphics::getInput()
{
    arc::Event event;
    event.type = EventType::None;
    event.key = 0;
    event.mouseX = 0;
    event.mouseY = 0;

    int ch = getch();
    if (ch != ERR)
        _processKeyboardEvent(ch, event);

    return event;
}

int arc::NcursesGraphics::getWidth() const
{
    return _width;
}

int arc::NcursesGraphics::getHeight() const
{
    return _height;
}

bool arc::NcursesGraphics::isKeyPressed(int key) const
{
    (void)key;
    return false;
}

extern "C" {
    arc::IGraphics* createGraphics()
    {
        return new arc::NcursesGraphics();
    }

    void destroyGraphics(arc::IGraphics* graphics)
    {
        delete graphics;
    }
}

