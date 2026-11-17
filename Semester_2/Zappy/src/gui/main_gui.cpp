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
#include "gui/InfoPanel.hpp"
#include "gui/Narrator.hpp"
#include "gui/Tutorial.hpp"
#include "gui/Director.hpp"
#include "gui/Capture.hpp"
#include "gui/UiText.hpp"
#include "gui/Lang.hpp"
#include "gui/GpuDraw.hpp"
#include "gui/Keys.hpp"
#include <raylib.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

enum class ConnState {
    WaitingWelcome,
    Authenticating,
    InGame
};

// Mode d'affichage, choisi par la variable d'environnement ZAPPY_MODE pour
// garder la ligne de commande du sujet (-p port -h machine) :
//   (absent)  GUI classique ; H affiche le panneau d'explications
//   demo      panneau d'explications affiché (scripts de démo)
//   tutorial  visite guidée avec bulles, puis moments clés de la partie
//   video     plans de caméra automatiques, titres et sous-titres
// Langue : ZAPPY_LANG=en pour l'anglais (français par défaut), touche L
// pour basculer en cours de partie.
enum class GuiMode { Normal, Demo, Tutorial, Video };

static GuiMode guiModeFromEnv()
{
    const char *m = std::getenv("ZAPPY_MODE");
    std::string mode = m ? m : "";
    if (mode == "demo")
        return GuiMode::Demo;
    if (mode == "tutorial" || mode == "tutoriel")
        return GuiMode::Tutorial;
    if (mode == "video")
        return GuiMode::Video;
    return GuiMode::Normal;
}

// ZAPPY_SIZE=1920x1080 : taille de fenêtre (utile pour enregistrer).
static void windowSize(int &w, int &h)
{
    w = 1280;
    h = 720;
    const char *s = std::getenv("ZAPPY_SIZE");
    int sw = 0, sh = 0;
    if (s && std::sscanf(s, "%dx%d", &sw, &sh) == 2 && sw >= 640 && sh >= 360) {
        w = sw;
        h = sh;
    }
}

static bool runningUnderWsl()
{
    std::ifstream f("/proc/sys/kernel/osrelease");
    std::string release;
    std::getline(f, release);
    return release.find("microsoft") != std::string::npos
        || release.find("WSL") != std::string::npos;
}

// Cadence d'affichage : synchro verticale (VSync) par défaut, calée sur
// l'écran. Sous WSL, la VSync est ignorée : sans limite, le GUI calcule des
// images à un rythme qui ne colle pas à celui où WSLg les affiche (60/s par
// défaut), et l'animation saccade. On y cale donc la cadence sur la
// fréquence annoncée par WSLg. ZAPPY_FPS=N impose N images/s (0 : aucune
// limite). À appeler après InitWindow.
static int targetFps()
{
    if (const char *s = std::getenv("ZAPPY_FPS")) {
        int fps = std::atoi(s);
        return fps > 0 ? fps : 0;
    }
    if (!runningUnderWsl())
        return 0;
    int hz = GetMonitorRefreshRate(GetCurrentMonitor());
    return hz > 0 ? hz : 60;
}

// WSL2 : Mesa ne prend plus de lui-même son pilote d3d12 (le GPU de
// Windows) et retombe sur llvmpipe, un rendu logiciel sur le CPU, plusieurs
// fois plus lent. On demande d3d12, sauf si l'utilisateur a choisi un pilote
// (GALLIUM_DRIVER) ou le rendu logiciel. Il est d'abord essayé dans un
// processus fils : un pilote imposé qui échoue fait planter l'ouverture de
// la fenêtre au lieu de revenir au rendu logiciel.
static void preferWslGpu()
{
    if (std::getenv("GALLIUM_DRIVER") || std::getenv("LIBGL_ALWAYS_SOFTWARE"))
        return;
    if (access("/dev/dxg", F_OK) != 0
        || access("/usr/lib/wsl/lib/libd3d12.so", F_OK) != 0)
        return;     // pas sous WSL2, ou sans accès au GPU
    pid_t pid = fork();
    if (pid < 0)
        return;
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
        }
        setenv("GALLIUM_DRIVER", "d3d12", 1);
        SetTraceLogLevel(LOG_NONE);
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
        InitWindow(64, 64, "Zappy GPU probe");
        _exit(IsWindowReady() ? 0 : 1);
    }
    for (int i = 0; i < 100; ++i) {         // 5 s au plus
        int status = 0;
        if (waitpid(pid, &status, WNOHANG) == pid) {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                setenv("GALLIUM_DRIVER", "d3d12", 0);
            return;
        }
        usleep(50000);
    }
    kill(pid, SIGKILL);
    waitpid(pid, nullptr, 0);
}

int main(int argc, char **argv)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Args args;
    try {
        args = parseArgs(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }

    GuiMode     mode = guiModeFromEnv();
    World       world;
    Protocol    protocol(world);
    AppCamera   camera;
    MapRenderer renderer;
    Network     network;
    Menu        menu;
    InfoPanel   info;
    Narrator    narrator;
    Tutorial    tutorial;
    Director    director;
    ConnState   state = ConnState::WaitingWelcome;
    bool        video = (mode == GuiMode::Video);
    double      langToastUntil = 0.0;   // message après la touche L

    try {
        network.connect(args.host, args.port);
    } catch (const std::exception &e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return 84;
    }


    SetTraceLogLevel(LOG_NONE);
    preferWslGpu();

    int winW, winH;
    windowSize(winW, winH);
    SetConfigFlags(FLAG_VSYNC_HINT);
    if (video)
        SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(winW, winH, "Zappy");
    SetTargetFPS(targetFps());
    UiText::load();

    // Après InitWindow : la capture a besoin du contexte OpenGL.
    Capture capture;

    if (mode == GuiMode::Normal || video)
        narrator.toggle();      // masqué (H pour l'afficher hors vidéo)
    if (video)
        info.setVisible(false);

    GpuDraw::load();        // avant les modèles (skinning GPU des personnages)
    std::clog << "[GPU] OpenGL : " << GpuDraw::rendererName() << "\n";
    if (GpuDraw::softwareRenderer())
        std::clog << "[GPU] Rendu logiciel : la carte graphique n'est pas "
                     "utilisée, les FPS seront bas.\n";
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

            // Le serveur envoie son état courant (carte, équipes, joueurs
            // déjà en jeu) : rejoindre une partie en cours reprend donc là
            // où elle en est, sans repartir de zéro.
            if (state == ConnState::Authenticating && world.width > 0) {
                state = ConnState::InGame;
            }
        }

        if (state == ConnState::InGame && !cameraInit && world.width > 0) {
            camera.init(world.width  * MapRenderer::TILE_SIZE,
                        world.height * MapRenderer::TILE_SIZE);
            cameraInit = true;
            if (mode == GuiMode::Tutorial)
                tutorial.start();
        }

        if (!video && IsKeyPressed(KEY_H))
            narrator.toggle();
        if (IsKeyPressed(KEY_L)) {
            Lang::toggle();
            langToastUntil = GetTime() + 2.0;
        }
        menu.setRightMargin(narrator.visible() ? Narrator::PANEL_W + 30 : 20);

        TutorialContext tctx{world, narrator, renderer, camera,
                             info.overviewRect(), menu.teamsRect(),
                             narrator.panelRect(), menu.isActive(),
                             menu.watchedPlayerId()};

        if (cameraInit) {
            // Mode "watch" : la caméra orbite autour du joueur choisi dans
            // le panneau Équipes & Joueurs (position interpolée de marche).
            int watchId = menu.watchedPlayerId();
            if (watchId >= 0) {
                auto it = world.players.find(watchId);
                if (it == world.players.end()) {
                    menu.clearWatch();  // mort disparu ou déconnecté
                } else {
                    Vector2 vis = renderer.getPlayerVisualPos(
                        watchId, it->second.x, it->second.y);
                    camera.follow({vis.x * MapRenderer::TILE_SIZE, 1.0f,
                                   vis.y * MapRenderer::TILE_SIZE});
                }
            }
            bool mouseOnHud = menu.isMouseCaptured() || info.containsMouse()
                           || narrator.containsMouse()
                           || tutorial.containsMouse();
            camera.update(!mouseOnHud && !video);
            info.updateHover(world, camera.get(), !mouseOnHud && !video);
            // Le narrateur lit les événements avant que les effets ne
            // les consomment.
            narrator.observe(world);
            if (video)
                director.update(world, narrator, renderer, camera);
            tutorial.update(tctx);
            renderer.update(world, GetFrameTime());
        }

        if (!video && Keys::menuPressed()) {
            menu.toggle();
        }
        if (!video && IsKeyPressed(KEY_I)) {
            info.toggle();
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (cameraInit) {
            BeginMode3D(camera.get());
            renderer.draw(world, info.hoverX(), info.hoverY()); // Rendu 3D
            EndMode3D();
            renderer.drawOverlay(camera.get());
        } else {
            DrawText("Connecting and loading assets...", 10, 50, 20, WHITE);
        }

        if (!video) {
            DrawText(("Host: " + args.host + "  Port: " + std::to_string(args.port)).c_str(),
                      10, 10, 18, WHITE);
            DrawFPS(10, 34);
        }

        if (cameraInit && !video) {
            info.draw(world);
            narrator.drawPanel(world);
        }
        if (!video)
            menu.draw(world);

        if (cameraInit) {
            if (video) {
                director.draw(world, narrator);
            } else {
                narrator.drawVictory(world);
                // Rectangles à jour (les panneaux viennent d'être dessinés).
                tctx.infoRect     = info.overviewRect();
                tctx.menuRect     = menu.teamsRect();
                tctx.narratorRect = narrator.panelRect();
                tutorial.draw(tctx);
            }
        }
        if (GetTime() < langToastUntil) {
            const char *msg = Lang::T("Langue : français", "Language: English");
            float w = UiText::width(msg, 22, true) + 40;
            Rectangle box = {(GetScreenWidth() - w) / 2.0f, 24, w, 44};
            DrawRectangleRounded(box, 0.4f, 8, {12, 14, 24, 220});
            UiText::draw(msg, box.x + 20, box.y + 10, 22, {255, 203, 0, 255},
                         true);
        }

        capture.frame();
        EndDrawing();

        if (director.wantsExit())
            break;
        // Le serveur s'arrête dès la victoire : on garde alors la fenêtre
        // ouverte pour montrer la fin (bannière, carton du mode vidéo).
        if (!network.isConnected() && state == ConnState::InGame
            && !world.gameOver) {
            std::cerr << "Disconnected from server" << std::endl;
            break;
        }
    }

    capture.stopRecording();
    UiText::unload();
    GpuDraw::unload();
    CloseWindow();
    return 0;
}
