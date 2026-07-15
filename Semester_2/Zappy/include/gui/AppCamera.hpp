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
    void           update();
    const Camera3D &get() const;

private:
    Camera3D _cam    = {};
    float    _yaw    = 45.0f;
    float    _pitch  = 45.0f;
    float    _radius = 20.0f;
    Vector3  _target = {};

    void applyOrbit();
};
