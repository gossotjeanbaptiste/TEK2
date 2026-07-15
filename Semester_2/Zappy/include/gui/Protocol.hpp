/*
** EPITECH PROJECT, 2026
** Protocol.hpp
** File description:
** GUI protocol parser — dispatches received server lines into World state
*/

#pragma once

#include "gui/World.hpp"
#include <sstream>
#include <string>
#include <unordered_map>

class Protocol {
public:
    explicit Protocol(World &world);
    ~Protocol() = default;

    void dispatch(const std::string &line);

private:
    World &_world;

    // Type de pointeur sur fonction membre pour le routage des commandes
    using ParseFunc = void (Protocol::*)(std::istringstream &);

    // Table de hachage associant la chaîne de la commande à sa fonction de parsing
    std::unordered_map<std::string, ParseFunc> _handlers;

    // Enregistrement initial des commandes dans la table
    void registerHandlers();

    // Méthodes de parsing du protocole de base
    void parseMsz(std::istringstream &ss);
    void parseBct(std::istringstream &ss);
    void parseTna(std::istringstream &ss);
    void parsePnw(std::istringstream &ss);
    void parsePpo(std::istringstream &ss);
    void parsePlv(std::istringstream &ss);
    void parsePin(std::istringstream &ss);
    void parsePdi(std::istringstream &ss);
    void parseSgt(std::istringstream &ss);
    void parseSst(std::istringstream &ss);
    void parseSeg(std::istringstream &ss);
    void parseSmg(std::istringstream &ss);

    // Méthodes de parsing des événements de gameplay (Ajouts)
    void parsePex(std::istringstream &ss);
    void parsePbc(std::istringstream &ss);
    void parsePic(std::istringstream &ss);
    void parsePie(std::istringstream &ss);
    void parsePfk(std::istringstream &ss);
    void parsePdr(std::istringstream &ss);
    void parsePgt(std::istringstream &ss);
    void parseEnw(std::istringstream &ss);
    void parseEbo(std::istringstream &ss);
    void parseEdi(std::istringstream &ss);
};