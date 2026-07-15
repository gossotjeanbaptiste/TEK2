/*
** EPITECH PROJECT, 2026
** main_gui.cpp
** File description:
** Entry point for zappy_gui with smart asset loading and HUD Menu
*/

#include "gui/Args.hpp"
#include "gui/World.hpp"
#include "gui/Protocol.hpp"
#include "gui/AppCamera.hpp"
#include "gui/MapRenderer.hpp"
#include "gui/Network.hpp"
#include "gui/Menu.hpp"
#include <raylib.h>
#include <iostream>
#include <string>

enum class ConnState {
    WaitingWelcome,
    Authenticating,
    InGame
};

int main(int argc, char **argv)
{
    Args args;
    try {
        args = parseArgs(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }

    World       world;
    Protocol    protocol(world);
    AppCamera   camera;
    MapRenderer renderer;
    Network     network;
    Menu        menu; 
    ConnState   state = ConnState::WaitingWelcome;

    try {
        network.connect(args.host, args.port);
    } catch (const std::exception &e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return 84;
    }
    
    SetTraceLogLevel(LOG_NONE);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Zappy");
    SetTargetFPS(60);

    
    renderer.loadAssets(); 

    bool cameraInit = false;

    while (!WindowShouldClose()) {
        network.poll();

        if (!network.isConnected() && state != ConnState::InGame) {
            std::cerr << "Lost connection to server before handshake completed" << std::endl;
            break;
        }

        while (network.hasLine()) {
            std::string line = network.popLine();

            if (state == ConnState::WaitingWelcome) {
                if (line == "WELCOME") {
                    network.send("GRAPHIC");
                    state = ConnState::Authenticating;
                }
                continue;
            }

            protocol.dispatch(line);

            if (state == ConnState::Authenticating && world.width > 0) {
                state = ConnState::InGame;
            }
        }

        if (state == ConnState::InGame && !cameraInit && world.width > 0) {
            camera.init(world.width  * MapRenderer::TILE_SIZE,
                        world.height * MapRenderer::TILE_SIZE);
            cameraInit = true;
        }

        if (cameraInit)
            camera.update();

        if (IsKeyPressed(KEY_M)) {
            menu.toggle();
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (cameraInit) {
            BeginMode3D(camera.get());
            renderer.draw(world); // Rendu 3D
            EndMode3D();
        } else {
            DrawText("Connecting and loading assets...", 10, 50, 20, WHITE);
        }

        DrawText(("Host: " + args.host + "  Port: " + std::to_string(args.port)).c_str(),
                  10, 10, 18, WHITE);
        DrawFPS(10, 34);
        
        menu.draw(world, renderer.getResourceColors(), camera.get());

        EndDrawing();

        if (!network.isConnected() && state == ConnState::InGame) {
            std::cerr << "Disconnected from server" << std::endl;
            break;
        }
    }

    // Les modèles GPU doivent être libérés avant la fermeture du contexte
    renderer.unloadAssets();
    CloseWindow();
    return 0;
}