/*
** EPITECH PROJECT, 2026
** Capture.cpp
** File description:
** Screenshots (F12 / periodic) and MP4 recording through an ffmpeg pipe
*/

#include "gui/Capture.hpp"
#include <raylib.h>
#include <rlgl.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>

namespace {
    constexpr int FPS = 30;

    std::string env(const char *name, const std::string &def = "")
    {
        const char *v = std::getenv(name);
        return (v && *v) ? std::string(v) : def;
    }
}

Capture::Capture()
{
    _dir = env("ZAPPY_SHOTS", "screenshots");
    _every = std::atof(env("ZAPPY_SHOT_EVERY", "0").c_str());
    _recordPath = env("ZAPPY_RECORD");
    _ffmpeg = env("ZAPPY_FFMPEG", "ffmpeg");
    // ffmpeg qui s'arrête ne doit pas tuer le GUI (écriture dans un tube
    // fermé).
    std::signal(SIGPIPE, SIG_IGN);
}

Capture::~Capture()
{
    stopRecording();
}

std::string Capture::screenshot(const std::string &label)
{
    mkdir(_dir.c_str(), 0755);
    char name[256];
    std::snprintf(name, sizeof(name), "%s/zappy_%03d_%06.1fs%s%s.png",
                  _dir.c_str(), ++_shotCount, GetTime(),
                  label.empty() ? "" : "_", label.c_str());
    Image img = LoadImageFromScreen();
    bool ok = ExportImage(img, name);
    UnloadImage(img);
    if (!ok)
        return "";
    std::clog << "[Capture] " << name << std::endl;
    return name;
}

void Capture::startRecording(int w, int h)
{
    _recordTried = true;
    _recW = w;
    _recH = h;
    // Tailles paires obligatoires pour yuv420p ; faststart : lisible en
    // streaming (réseaux sociaux).
    std::string cmd = "\"" + _ffmpeg + "\" -y -loglevel error"
        " -f rawvideo -pix_fmt rgba -s " + std::to_string(w) + "x"
        + std::to_string(h) + " -r " + std::to_string(FPS) + " -i -"
        " -vf \"scale=trunc(iw/2)*2:trunc(ih/2)*2\""
        " -c:v libx264 -preset ultrafast -crf 18 -pix_fmt yuv420p"
        " -movflags +faststart \"" + _recordPath + "\"";
    _pipe = popen(cmd.c_str(), "w");
    if (!_pipe) {
        std::cerr << "[Capture] impossible de lancer ffmpeg : enregistrement "
                     "désactivé" << std::endl;
        return;
    }
    _recStart  = GetTime();
    _framesOut = 0;
    std::clog << "[Capture] enregistrement → " << _recordPath << std::endl;
}

void Capture::stopRecording()
{
    if (!_pipe)
        return;
    int status = pclose(_pipe);
    _pipe = nullptr;
    if (status == 0)
        std::clog << "[Capture] vidéo écrite : " << _recordPath << " ("
                  << _framesOut << " images dont " << _uniqueFrames
                  << " uniques)" << std::endl;
    else
        std::cerr << "[Capture] ffmpeg a échoué (code " << status
                  << ") : la vidéo est peut-être incomplète" << std::endl;
}

void Capture::frame()
{
    bool wantShot = IsKeyPressed(KEY_F12)
        || (_every > 0.0 && GetTime() - _lastShot >= _every);
    if (!wantShot && _recordPath.empty())
        return;
    // Le texte et les formes 2D sont mis en lot et envoyés au GPU seulement
    // dans EndDrawing : on vide le lot avant de lire l'écran.
    rlDrawRenderBatchActive();
    if (IsKeyPressed(KEY_F12))
        screenshot();
    if (_every > 0.0 && GetTime() - _lastShot >= _every) {
        _lastShot = GetTime();
        screenshot("auto");
    }

    if (_recordPath.empty())
        return;
    if (!_pipe && !_recordTried) {
        Image probe = LoadImageFromScreen();
        startRecording(probe.width, probe.height);
        UnloadImage(probe);
    }
    if (!_pipe)
        return;

    // Vidéo en temps réel : si une frame a pris du retard, on la duplique
    // pour garder la bonne durée.
    long due = static_cast<long>((GetTime() - _recStart) * FPS) + 1;
    long todo = due - _framesOut;
    if (todo <= 0)
        return;
    // Jusqu'à 2 s de rattrapage : la vidéo garde la durée réelle même si
    // le rendu ralentit (lecture de l'écran en 1080p, encodage).
    if (todo > 2 * FPS)
        todo = 2 * FPS;
    Image img = LoadImageFromScreen();
    if (img.width != _recW || img.height != _recH) {
        // Fenêtre redimensionnée : on garde la taille d'origine.
        ImageResize(&img, _recW, _recH);
    }
    size_t bytes = static_cast<size_t>(img.width) * img.height * 4;
    for (long i = 0; i < todo; ++i) {
        if (fwrite(img.data, 1, bytes, _pipe) != bytes) {
            std::cerr << "[Capture] ffmpeg ne répond plus, arrêt de "
                         "l'enregistrement" << std::endl;
            UnloadImage(img);
            stopRecording();
            return;
        }
    }
    _framesOut = due;
    _uniqueFrames++;
    UnloadImage(img);
}
