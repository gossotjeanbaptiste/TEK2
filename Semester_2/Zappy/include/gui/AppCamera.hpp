/*
** EPITECH PROJECT, 2026
** AppCamera.hpp
** File description:
** Orbital 3D camera with mouse and keyboard controls
*/

#pragma once
#include <raylib.h>

class AppCamera {
public:
    void           init(float mapW, float mapH);
    // allowMouseInput = false quand la souris est capturée par le HUD
    // (clic dans un panneau) : la caméra ignore alors clics et molette.
    void           update(bool allowMouseInput = true);
    // Mode "watch" : recentre en douceur l'orbite sur ce point (appelé
    // chaque frame avec la position du joueur suivi ; zoom et rotation
    // restent libres, on orbite autour de lui).
    void           follow(Vector3 point);
    const Camera3D &get() const;

    // Mouvements automatiques (tutoriel, mode vidéo) : la caméra glisse en
    // douceur vers ce cadrage ; pitch < 0 garde l'inclinaison actuelle.
    void  setShot(Vector3 target, float radius, float pitch = -1.0f);
    void  clearShot() { _hasShot = false; }
    bool  hasShot() const { return _hasShot; }
    // Rotation lente autour de la cible (degrés / seconde, 0 = arrêt).
    void  setAutoOrbit(float degPerSec) { _orbitSpeed = degPerSec; }
    // Cadrage « toute la carte » calculé par init().
    Vector3 mapCenter() const { return _mapCenter; }
    float   mapRadius() const { return _mapRadius; }
    // Avance les mouvements automatiques (appelé par update()).
    void  animate(float dt);

private:
    Camera3D _cam    = {};
    float    _yaw    = 45.0f;
    float    _pitch  = 45.0f;
    float    _radius = 20.0f;
    Vector3  _target = {};

    bool     _hasShot     = false;
    Vector3  _shotTarget  = {};
    float    _shotRadius  = 20.0f;
    float    _shotPitch   = -1.0f;
    float    _orbitSpeed  = 0.0f;
    Vector3  _mapCenter   = {};
    float    _mapRadius   = 20.0f;

    void applyOrbit();
};
