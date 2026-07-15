/*
** EPITECH PROJECT, 2026
** Egg.hpp
** File description:
** Egg structures for Zappy GUI
*/

#pragma once

struct Egg {
    int id       = 0; // ID unique de l'œuf (fourni par le serveur via #e)
    int parentId = 0; // ID du joueur qui a pondu l'œuf (#n)
    int x        = 0; // Position X de l'œuf sur la grille
    int y        = 0; // Position Y de l'œuf sur la grille
};