/*
** EPITECH PROJECT, 2026
** AppCamera.cpp
** File description:
** Orbital 3D camera with mouse and keyboard controls
*/

#include "gui/AppCamera.hpp"
#include <raymath.h>
#include <cmath>

static constexpr float MIN_RADIUS = 4.0f;
static constexpr float MAX_RADIUS = 150.0f;
static constexpr float MIN_PITCH  = 5.0f;
static constexpr float MAX_PITCH  = 89.0f;

void AppCamera::init(float mapW, float mapH)
{
    _target = {mapW / 2.0f, 0.0f, mapH / 2.0f};
    _radius = (mapW > mapH ? mapW : mapH) * 0.9f;
    if (_radius < MIN_RADIUS) _radius = MIN_RADIUS;
    if (_radius > MAX_RADIUS) _radius = MAX_RADIUS;
    _cam.up         = {0.0f, 1.0f, 0.0f};
    _cam.fovy       = 45.0f;
    _cam.projection = CAMERA_PERSPECTIVE;
    applyOrbit();
}

void AppCamera::update()
{
    // Scroll = zoom (proportional to distance)
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        _radius -= wheel * _radius * 0.1f;
        if (_radius < MIN_RADIUS) _radius = MIN_RADIUS;
        if (_radius > MAX_RADIUS) _radius = MAX_RADIUS;
    }

    // Left-click drag = rotate
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        _yaw   -= delta.x * 0.3f;
        _pitch -= delta.y * 0.3f;
        if (_pitch < MIN_PITCH) _pitch = MIN_PITCH;
        if (_pitch > MAX_PITCH) _pitch = MAX_PITCH;
    }

    // Right-click drag = pan lateral
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta   = GetMouseDelta();
        Vector3 forward = Vector3Normalize({
            _target.x - _cam.position.x, 0.0f,
            _target.z - _cam.position.z});
        Vector3 right = Vector3Normalize(
            Vector3CrossProduct(forward, _cam.up));
        float speed = _radius * 0.003f;
        _target = Vector3Add(_target, Vector3Scale(right,   -delta.x * speed));
        _target = Vector3Add(_target, Vector3Scale(forward, -delta.y * speed));
    }

    // ZQSD = pan lateral (AZERTY)
    float keySpeed = _radius * 0.015f;
    Vector3 forward = Vector3Normalize({
        _target.x - _cam.position.x, 0.0f,
        _target.z - _cam.position.z});
    Vector3 right = Vector3Normalize(
        Vector3CrossProduct(forward, _cam.up));
    if (IsKeyDown(KEY_Z) || IsKeyDown(KEY_W))
        _target = Vector3Add(_target, Vector3Scale(forward, keySpeed));
    if (IsKeyDown(KEY_S))
        _target = Vector3Add(_target, Vector3Scale(forward, -keySpeed));
    if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_A))
        _target = Vector3Add(_target, Vector3Scale(right, -keySpeed));
    if (IsKeyDown(KEY_D))
        _target = Vector3Add(_target, Vector3Scale(right, keySpeed));

    applyOrbit();
}

const Camera3D &AppCamera::get() const
{
    return _cam;
}

void AppCamera::applyOrbit()
{
    float yawRad   = _yaw   * DEG2RAD;
    float pitchRad = _pitch * DEG2RAD;
    _cam.position = {
        _target.x + _radius * cosf(pitchRad) * sinf(yawRad),
        _target.y + _radius * sinf(pitchRad),
        _target.z + _radius * cosf(pitchRad) * cosf(yawRad)
    };
    _cam.target = _target;
}
