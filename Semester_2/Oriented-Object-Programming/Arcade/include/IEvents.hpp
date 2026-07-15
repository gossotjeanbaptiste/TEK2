/*
** EPITECH PROJECT, 2024
** Arcade
** File description:
** Event types for input handling
*/

#pragma once

namespace arc {

    enum class EventType {
        KeyPressed,
        KeyReleased,
        MousePressed,
        MouseMoved,
        Quit,
        None
    };

    struct Event {
        EventType type;
        int key;
        int mouseX;
        int mouseY;
    };

}
