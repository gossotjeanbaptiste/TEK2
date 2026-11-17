/*
** EPITECH PROJECT, 2026
** Capture.hpp
** File description:
** Screenshots (F12 / periodic) and MP4 recording through an ffmpeg pipe
*/

#pragma once

#include <cstdio>
#include <string>

// Configuré par variables d'environnement (le binaire garde les seuls
// arguments du sujet, -p et -h) :
//   ZAPPY_SHOTS=<dossier>     dossier des captures (défaut : screenshots)
//   ZAPPY_SHOT_EVERY=<s>      capture automatique toutes les s secondes
//   ZAPPY_RECORD=<fichier>    enregistre la fenêtre en vidéo (MP4, 30 i/s)
//   ZAPPY_FFMPEG=<binaire>    chemin de ffmpeg (défaut : ffmpeg du PATH)
class Capture {
public:
    Capture();
    ~Capture();

    // À appeler juste AVANT EndDrawing : l'image est encore dans le
    // tampon arrière.
    void frame();
    // Capture immédiate (renvoie le chemin écrit, vide si échec).
    std::string screenshot(const std::string &label = "");
    bool recording() const { return _pipe != nullptr; }
    void stopRecording();

private:
    void startRecording(int w, int h);

    std::string _dir         = "screenshots";
    double      _every       = 0.0;
    double      _lastShot    = 0.0;
    int         _shotCount   = 0;

    std::string _recordPath;
    std::string _ffmpeg      = "ffmpeg";
    FILE       *_pipe        = nullptr;
    bool        _recordTried = false;
    double      _recStart    = 0.0;
    long        _framesOut   = 0;
    long        _uniqueFrames = 0;
    int         _recW        = 0;
    int         _recH        = 0;
};
