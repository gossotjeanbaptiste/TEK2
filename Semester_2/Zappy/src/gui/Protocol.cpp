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
    // Le sujet envoie une ligne "tna N" par équipe, mais le serveur du repo
    // les met toutes sur une seule ligne ("tna t1 t2") : on lit tous les
    // noms. Sinon seule la première équipe était connue, les autres
    // tombaient sur la couleur par défaut et toutes les gemmes se
    // ressemblaient.
    std::string name;
    while (ss >> name) {
        bool known = false;
        for (const auto &t : _world.teams)
            known = known || (t == name);
        if (!known)
            _world.teams.push_back(name);
    }
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
    _world.fxEvents.push_back({FxType::Spawn, id, x, y, l});
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
    if (level > it->second.level)
        _world.fxEvents.push_back({FxType::LevelUp, id, it->second.x, it->second.y, level});
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
    int id  = parseId(idToken);
    auto it = _world.players.find(id);
    if (it == _world.players.end()) return;
    it->second.alive = false;
    _world.animEvents.push_back({id, AnimAction::Death});
    _world.fxEvents.push_back({FxType::Death, id, it->second.x, it->second.y, 0});
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

    auto srcIt = _world.players.find(id);
    if (srcIt == _world.players.end()) return;

    static const AnimAction EJECT_MOVES[4] = {
        AnimAction::KickLeft, AnimAction::KickRight,
        AnimAction::PunchLeft, AnimAction::PunchRight
    };
    _world.animEvents.push_back({id, EJECT_MOVES[std::rand() % 4]});
    _world.fxEvents.push_back({FxType::Eject, id, srcIt->second.x, srcIt->second.y, 0});

    int srcX = srcIt->second.x;
    int srcY = srcIt->second.y;
    for (const auto &pair : _world.players) {
        if (pair.first == id) continue;
        if (pair.second.x == srcX && pair.second.y == srcY)
            _world.animEvents.push_back({pair.first, AnimAction::HitReceiveEject});
    }
}

void Protocol::parsePbc(std::istringstream &ss)
{
    std::string idToken, message;
    if (!(ss >> idToken)) return;
    std::getline(ss, message);
    if (!message.empty() && message[0] == ' ') message.erase(0, 1);

    int id = parseId(idToken);
    _world.lastMessage = "Player #" + std::to_string(id) + " broadcasted: " + message;
    _world.animEvents.push_back({id, AnimAction::Wave});
    _world.fxEvents.push_back({FxType::Broadcast, id, 0, 0, 0});
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

    for (int pid : participantIds)
        _world.animEvents.push_back({pid, AnimAction::Roll});
    _world.fxEvents.push_back({FxType::IncantStart, -1, t.x, t.y, level});
}

void Protocol::parsePie(std::istringstream &ss)
{
    int x, y, result;
    if (!(ss >> x >> y >> result)) return;

    Tile &t = _world.tileAt(x, y);
    t.isIncanting = false;
    _world.fxEvents.push_back({FxType::IncantEnd, -1, t.x, t.y, result});

    _world.lastMessage = "Incantation at (" + std::to_string(x) + "," +
                         std::to_string(y) + ") ended with code " + std::to_string(result);

    for (const auto &pair : _world.players) {
        if (pair.second.x == x && pair.second.y == y)
            _world.animEvents.push_back({pair.first, AnimAction::RollEnd});
    }
}

void Protocol::parsePfk(std::istringstream &ss)
{
    std::string idToken;
    if (!(ss >> idToken)) return;
    int id = parseId(idToken);
    _world.animEvents.push_back({id, AnimAction::SwordSlash});
    _world.fxEvents.push_back({FxType::Fork, id, 0, 0, 0});
}

void Protocol::parsePdr(std::istringstream &ss)
{
    std::string idToken;
    int resourceId;
    if (!(ss >> idToken >> resourceId)) return;
    int id = parseId(idToken);
    _world.animEvents.push_back({id, AnimAction::Interact});
    _world.fxEvents.push_back({FxType::Drop, id, 0, 0, resourceId});
}

void Protocol::parsePgt(std::istringstream &ss)
{
    std::string idToken;
    int resourceId;
    if (!(ss >> idToken >> resourceId)) return;
    int id = parseId(idToken);
    _world.animEvents.push_back({id, AnimAction::Interact});
    _world.fxEvents.push_back({FxType::Pickup, id, 0, 0, resourceId});
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
    _world.fxEvents.push_back({FxType::EggLaid, playerId, x, y, 0});
}

void Protocol::parseEbo(std::istringstream &ss)
{
    std::string eggToken;
    if (!(ss >> eggToken)) return;
    eraseEgg(parseId(eggToken), FxType::EggHatch);
}

void Protocol::parseEdi(std::istringstream &ss)
{
    std::string eggToken;
    if (!(ss >> eggToken)) return;
    eraseEgg(parseId(eggToken), FxType::EggDie);
}

void Protocol::eraseEgg(int eggId, FxType fx)
{
    auto it = _world.eggs.find(eggId);
    if (it == _world.eggs.end()) return;
    _world.fxEvents.push_back({fx, -1, it->second.x, it->second.y, 0});
    _world.eggs.erase(it);
}