/*
** EPITECH PROJECT, 2026
** Keys.hpp
** File description:
** Keyboard bindings that work on both AZERTY and QWERTY layouts
*/

#pragma once

#include <raylib.h>

// raylib nomme les touches d'après leur position sur un clavier QWERTY
// (KEY_W = la touche à la place du W américain). Selon le système, le Z
// d'un AZERTY remonte donc comme KEY_W (position physique : cas de Linux,
// WSL, Windows, macOS) ou comme KEY_Z (certains serveurs X). En acceptant
// les deux jeux, ZQSD (AZERTY) et WASD (QWERTY) marchent partout, sans
// réglage ; les flèches aussi.
namespace Keys {
    inline bool forward()
    {
        return IsKeyDown(KEY_W) || IsKeyDown(KEY_Z) || IsKeyDown(KEY_UP);
    }

    inline bool backward()
    {
        return IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    }

    inline bool left()
    {
        return IsKeyDown(KEY_A) || IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT);
    }

    inline bool right()
    {
        return IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    }

    inline bool anyMove()
    {
        return forward() || backward() || left() || right();
    }

    // Menu M : sur AZERTY, la touche M est à la place du « ; » américain.
    inline bool menuPressed()
    {
        return IsKeyPressed(KEY_M) || IsKeyPressed(KEY_SEMICOLON);
    }
}
