# Zappy

Zappy is a multiplayer networked game where several teams compete on a toroidal map filled with resources. The goal is to bring at least 6 players from the same team to the maximum elevation level before the other teams. The project is split into three binaries: `zappy_server`, `zappy_gui`, and `zappy_ai`.

## Project objective

The server generates a living world with resources, players, eggs, and game actions.
The graphical client allows observing the world state in real time.
The AI client controls a player automatically using the commands allowed by the protocol.

## Overall architecture

The project is separated into three parts:

- **Server**: handles game logic, TCP connections, events, and rules.
- **GUI**: displays the map, players, resources, and events.
- **AI**: decides player actions and interacts with the server autonomously.

The server must run as a **single process and single thread**, using `poll` to multiplex sockets without busy waiting.

## World and resources

The world is a wrapping map: if a player leaves to the right, they reappear on the left, and similarly for top and bottom.
Available resources are: `food`, `linemate`, `deraumere`, `sibur`, `mendiane`, `phiras`, and `thystame`.
The server spawns resources at startup and every 20 time units, distributed according to map size and per-resource density.

## Game rules

Each player must primarily:

- eat;
- gather stones;
- participate in elevations to level up.

An elevation requires a certain number of players at the same level and a specific set of resources on the same tile. When conditions are met, the incantation begins, the involved players are frozen, and the level increases if the conditions stay valid until the ritual ends.

## Player commands

The player may only use the commands listed in the project specification:

- `Forward`
- `Right`
- `Left`
- `Look`
- `Inventory`
- `Broadcast text`
- `Connect_nbr`
- `Fork`
- `Eject`
- `Take`
- `Set`
- `Incantation`

Each command has an execution time depending on factor `f`, and the server must respond `ko` to any invalid or unknown command.

## Network communication

The AI client connects to the server over TCP.
The handshake sequence is:

1. the client opens a connection to the server port;
2. the server sends `WELCOME`;
3. the client sends the team name;
4. the server replies with the number of available slots and then the map dimensions.

The client can send up to 10 requests in a row without waiting for responses. The server then processes commands in the order received.

## Graphical interface

The GUI must present a representation of the world, at least in 2D, with icons or visual elements that clearly show the map. The GUI protocol defines messages such as `msz`, `bct`, `tna`, `pnw`, `ppo`, `plv`, `pin`, `pex`, `pbc`, `pic`, `pie`, `pfk`, `pdr`, `pgt`, `pdi`, `enw`, `ebo`, `edi`, `sgt`, `sst`, `seg`, `smg`, `suc`, and `sbp`.

The GUI client authenticates by sending `GRAPHIC` instead of a team name when the server expects identification.

## Project organization

A simple division can be:

- **Server**
  - Networking and sockets;
  - Game engine;
  - Resource and event management.
- **GUI**
  - Map rendering;
  - Real-time updates;
  - Visualization of players, eggs and resources.
- **AI**
  - Movement strategy;
  - Resource collection;
  - Survival and elevation management.

## Build

The project must provide three main rules in the `Makefile`:

- `zappy_server`
- `zappy_gui`
- `zappy_ai`

## License / usage

This repository contains an implementation of the Zappy project inspired by the official specifications.
The goal is to provide a clean, readable, and maintainable base for the server, the GUI client, and the AI.

---