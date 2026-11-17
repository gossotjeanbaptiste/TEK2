/*
** EPITECH PROJECT, 2026
** UiText.hpp
** File description:
** Shared UI font (DejaVu, Latin-1 + a few symbols) and text helpers
*/

#pragma once

#include <raylib.h>
#include <string>
#include <vector>

// Police des surcouches de démo / tutoriel / vidéo. Charge DejaVu depuis
// assets/fonts (accents, flèches, puces) ; sans le fichier, retombe sur la
// police par défaut de raylib (les symboles hors Latin-1 deviennent '?').
namespace UiText {
    void load();
    void unload();

    void  draw(const std::string &text, float x, float y, float size,
               Color color, bool bold = false);
    // Texte avec ombre portée : lisible par-dessus la scène 3D.
    void  drawShadow(const std::string &text, float x, float y, float size,
                     Color color, bool bold = false);
    float width(const std::string &text, float size, bool bold = false);
    // Découpe `text` en lignes de largeur <= maxWidth (sur les espaces,
    // '\n' force un retour).
    std::vector<std::string> wrap(const std::string &text, float size,
                                  float maxWidth, bool bold = false);
    // Dessine un paragraphe découpé ; renvoie la hauteur occupée.
    float drawWrapped(const std::string &text, float x, float y, float size,
                      float maxWidth, Color color, float lineGap = 1.25f,
                      bool bold = false);
}
