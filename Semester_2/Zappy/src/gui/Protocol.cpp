/*
** EPITECH PROJECT, 2026
** Protocol.cpp
** File description:
** GUI protocol parser using a Command Router lookup table
*/

#include "gui/Protocol.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

static int parseId(const std::string &token)
{
    const char *s = token.c_str();
    if (*s == '#') ++s;
    return std::atoi(s);
}

Protocol::Protocol(World &world) : _world(world)
{
    registerHandlers();
}

void Protocol::registerHandlers()
{
    // Commandes de base de l'environnement
    _handlers["msz"] = &Protocol::parseMsz;
    _handlers["bct"] = &Protocol::parseBct;
    _handlers["tna"] = &Protocol::parseTna;
    _handlers["sgt"] = &Protocol::parseSgt;
    _handlers["sst"] = &Protocol::parseSst;
    _handlers["seg"] = &Protocol::parseSeg;
    _handlers["smg"] = &Protocol::parseSmg;

    // Commandes d'actions et états des joueurs
    _handlers["pnw"] = &Protocol::parsePnw;
    _handlers["ppo"] = &Protocol::parsePpo;
    _handlers["plv"] = &Protocol::parsePlv;
    _handlers["pin"] = &Protocol::parsePin;
    _handlers["pdi"] = &Protocol::parsePdi;
    _handlers["pex"] = &Protocol::parsePex;
    _handlers["pbc"] = &Protocol::parsePbc;
    _handlers["pic"] = &Protocol::parsePic;
    _handlers["pie"] = &Protocol::parsePie;
    _handlers["pfk"] = &Protocol::parsePfk;
    _handlers["pdr"] = &Protocol::parsePdr;
    _handlers["pgt"] = &Protocol::parsePgt;

    // Commandes de cycle de vie des œufs
    _handlers["enw"] = &Protocol::parseEnw;
    _handlers["ebo"] = &Protocol::parseEbo;
    _handlers["edi"] = &Protocol::parseEdi;
}

void Protocol::dispatch(const std::string &line)
{
    std::istringstream ss(line);
    std::string cmd;
    if (!(ss >> cmd)) return;

    // Recherche O(1) de la fonction associée à la commande reçue
    auto it = _handlers.find(cmd);
    if (it != _handlers.end()) {
        ParseFunc func = it->second;
        (this->*func)(ss);
    }
}

void Protocol::parseMsz(std::istringstream &ss)
{
    int w, h;
    if (!(ss >> w >> h)) return;
    _world.resize(w, h);
}

void Protocol::parseBct(std::istringstream &ss)
{
    int x, y;
    if (!(ss >> x >> y) || _world.width == 0) return;
    Tile &t = _world.tileAt(x, y);
    for (int i = 0; i < RESOURCE_COUNT; ++i)
        ss >> t.resources[i];
}

void Protocol::parseTna(std::istringstream &ss)
{
    std::string name;
    if (!(ss >> name)) return;
    for (const auto &t : _world.teams)
        if (t == name) return;
    _world.teams.push_back(name);
}

void Protocol::parsePnw(std::istringstream &ss)
{
    std::string idToken, team;
    int x, y, o, l;
    if (!(ss >> idToken >> x >> y >> o >> l >> team)) return;
    int id        = parseId(idToken);
    Player &p     = _world.players[id];
    p.id          = id;
    p.x           = x;
    p.y           = y;
    p.orientation = static_cast<Orientation>(o);
    p.level       = l;
    p.team        = team;
    p.alive       = true;
}

void Protocol::parsePpo(std::istringstream &ss)
{
    std::string idToken;
    int x, y, o;
    if (!(ss >> idToken >> x >> y >> o)) return;
    int id  = parseId(idToken);
    auto it = _world.players.find(id);
    if (it == _world.players.end()) return;
    it->second.x           = x;
    it->second.y           = y;
    it->second.orientation = static_cast<Orientation>(o);
}

void Protocol::parsePlv(std::istringstream &ss)
{
    std::string idToken;
    int level;
    if (!(ss >> idToken >> level)) return;
    int id  = parseId(idToken);
    auto it = _world.players.find(id);
    if (it == _world.players.end()) return;
    it->second.level = level;
}

void Protocol::parsePin(std::istringstream &ss)
{
    std::string idToken;
    int x, y;
    if (!(ss >> idToken >> x >> y)) return;
    int id  = parseId(idToken);
    auto it = _world.players.find(id);
    if (it == _world.players.end()) return;
    it->second.x = x;
    it->second.y = y;
    for (int i = 0; i < RESOURCE_COUNT; ++i)
        ss >> it->second.inventory[i];
}

void Protocol::parsePdi(std::istringstream &ss)
{
    std::string idToken;
    if (!(ss >> idToken)) return;
    _world.players.erase(parseId(idToken));
}

void Protocol::parseSgt(std::istringstream &ss)
{
    ss >> _world.timeUnit;
}

void Protocol::parseSst(std::istringstream &ss)
{
    ss >> _world.timeUnit;
}

void Protocol::parseSeg(std::istringstream &ss)
{
    if (!(ss >> _world.winnerTeam)) return;
    _world.gameOver = true;
}

void Protocol::parseSmg(std::istringstream &ss)
{
    std::getline(ss, _world.lastMessage);
    if (!_world.lastMessage.empty() && _world.lastMessage[0] == ' ')
        _world.lastMessage.erase(0, 1);
}

void Protocol::parsePex(std::istringstream &ss)
{
    std::string idToken;
    if (!(ss >> idToken)) return;
    int id = parseId(idToken);
    _world.lastMessage = "Player #" + std::to_string(id) + " expelled everyone from their tile!";
}

void Protocol::parsePbc(std::istringstream &ss)
{
    std::string idToken, message;
    if (!(ss >> idToken)) return;
    std::getline(ss, message);
    if (!message.empty() && message[0] == ' ') message.erase(0, 1);
    
    int id = parseId(idToken);
    _world.lastMessage = "Player #" + std::to_string(id) + " broadcasted: " + message;
}

void Protocol::parsePic(std::istringstream &ss)
{
    int x, y, level;
    if (!(ss >> x >> y >> level)) return;
    
    std::string idToken;
    std::vector<int> participantIds;
    while (ss >> idToken) {
        participantIds.push_back(parseId(idToken));
    }
    
    Tile &t = _world.tileAt(x, y);
    t.isIncanting = true;
    t.incantationLevel = level;
}

void Protocol::parsePie(std::istringstream &ss)
{
    int x, y, result;
    if (!(ss >> x >> y >> result)) return;
    
    Tile &t = _world.tileAt(x, y);
    t.isIncanting = false;
    
    _world.lastMessage = "Incantation at (" + std::to_string(x) + "," + 
                         std::to_string(y) + ") ended with code " + std::to_string(result);
}

void Protocol::parsePfk(std::istringstream &ss)
{
    std::string idToken;
    if (!(ss >> idToken)) return;
    // Emplacement pour déclencher une animation "ponte" sur le joueur si nécessaire
}

void Protocol::parsePdr(std::istringstream &ss)
{
    std::string idToken;
    int resourceId;
    if (!(ss >> idToken >> resourceId)) return;
}

void Protocol::parsePgt(std::istringstream &ss)
{
    std::string idToken;
    int resourceId;
    if (!(ss >> idToken >> resourceId)) return;
}

void Protocol::parseEnw(std::istringstream &ss)
{
    std::string eggToken, playerToken;
    int x, y;
    if (!(ss >> eggToken >> playerToken >> x >> y)) return;
    
    int eggId = parseId(eggToken);
    int playerId = parseId(playerToken);
    
    Egg egg;
    egg.id = eggId;
    egg.parentId = playerId;
    egg.x = x;
    egg.y = y;
    _world.eggs[eggId] = egg;
}

void Protocol::parseEbo(std::istringstream &ss)
{
    std::string eggToken;
    if (!(ss >> eggToken)) return;
    _world.eggs.erase(parseId(eggToken));
}

void Protocol::parseEdi(std::istringstream &ss)
{
    std::string eggToken;
    if (!(ss >> eggToken)) return;
    _world.eggs.erase(parseId(eggToken));
}