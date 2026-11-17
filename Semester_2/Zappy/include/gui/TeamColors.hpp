/*
** EPITECH PROJECT, 2026
** TeamColors.hpp
** File description:
** Team color palette shared between 3D rendering and HUD
*/

#pragma once

#include <raylib.h>

// Couleur attribuée à chaque équipe (index donné par World::teamIndex).
// Utilisée par le rendu 3D (gemme au-dessus des joueurs) et le HUD.
// Les 8 premières teintes sont choisies pour rester bien distinctes une fois
// posées sur la gemme ; au-delà, on répartit les teintes par angle d'or.
// Index -1 (équipe inconnue du GUI) : gris neutre.
inline Color teamColorFromIndex(int index)
{
    static const Color PALETTE[] = {
        {230,  41,  55, 255},   // rouge
        { 30, 120, 255, 255},   // bleu
        { 40, 200,  70, 255},   // vert
        {255, 210,   0, 255},   // jaune
        {170,  60, 230, 255},   // violet
        {  0, 215, 215, 255},   // cyan
        {255, 120,   0, 255},   // orange
        {255, 255, 255, 255},   // blanc
    };
    const int count = static_cast<int>(sizeof(PALETTE) / sizeof(PALETTE[0]));

    if (index < 0)
        return {130, 130, 130, 255};
    if (index < count)
        return PALETTE[index];
    float hue = static_cast<float>((index * 137) % 360);
    return ColorFromHSV(hue, 0.85f, 0.95f);
}
