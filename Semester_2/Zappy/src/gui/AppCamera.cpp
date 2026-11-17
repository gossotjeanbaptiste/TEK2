/*
** EPITECH PROJECT, 2026
** AppCamera.cpp
** File description:
** Orbital 3D camera with mouse and keyboard controls
*/

#include "gui/AppCamera.hpp"
#include "gui/Keys.hpp"
#include <raymath.h>
#include <cmath>

static constexpr float MIN_RADIUS = 4.0f;
static constexpr float MAX_RADIUS = 150.0f;
static constexpr float MIN_PITCH  = 5.0f;
static constexpr float MAX_PITCH  = 89.0f;

// Durée de la dernière frame, bornée : un à-coup (chargement) ne doit pas
// faire bondir la caméra.
static float frameDelta()
{
    return fminf(GetFrameTime(), 0.1f);
}

void AppCamera::init(float mapW, float mapH)
{
    _target = {mapW / 2.0f, 0.0f, mapH / 2.0f};
    _radius = (mapW > mapH ? mapW : mapH) * 0.9f;
    if (_radius < MIN_RADIUS) _radius = MIN_RADIUS;
    if (_radius > MAX_RADIUS) _radius = MAX_RADIUS;
    _mapCenter = _target;
    _mapRadius = _radius;
    _cam.up         = {0.0f, 1.0f, 0.0f};
    _cam.fovy       = 45.0f;
    _cam.projection = CAMERA_PERSPECTIVE;
    applyOrbit();
}

void AppCamera::update(bool allowMouseInput)
{
    // Scroll = zoom (proportional to distance)
    float wheel = allowMouseInput ? GetMouseWheelMove() : 0.0f;
    if (wheel != 0.0f) {
        _radius -= wheel * _radius * 0.1f;
        if (_radius < MIN_RADIUS) _radius = MIN_RADIUS;
        if (_radius > MAX_RADIUS) _radius = MAX_RADIUS;
    }

    // Left-click drag = rotate
    if (allowMouseInput && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        _yaw   -= delta.x * 0.3f;
        _pitch -= delta.y * 0.3f;
        if (_pitch < MIN_PITCH) _pitch = MIN_PITCH;
        if (_pitch > MAX_PITCH) _pitch = MAX_PITCH;
    }

    // Right-click drag = pan lateral
    if (allowMouseInput && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
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

    // ZQSD / WASD / flèches = pan latéral (AZERTY et QWERTY, voir Keys.hpp).
    // Vitesse par seconde, la cadence n'étant plus fixe : même allure
    // qu'avant (0.015 x rayon par image à 60 images/s), à tout framerate.
    float keySpeed = _radius * 0.015f * 60.0f * frameDelta();
    Vector3 forward = Vector3Normalize({
        _target.x - _cam.position.x, 0.0f,
        _target.z - _cam.position.z});
    Vector3 right = Vector3Normalize(
        Vector3CrossProduct(forward, _cam.up));
    if (Keys::forward())
        _target = Vector3Add(_target, Vector3Scale(forward, keySpeed));
    if (Keys::backward())
        _target = Vector3Add(_target, Vector3Scale(forward, -keySpeed));
    if (Keys::left())
        _target = Vector3Add(_target, Vector3Scale(right, -keySpeed));
    if (Keys::right())
        _target = Vector3Add(_target, Vector3Scale(right, keySpeed));

    // L'utilisateur reprend la main : on abandonne le cadrage automatique.
    bool userMoved = wheel != 0.0f
        || (allowMouseInput && (IsMouseButtonDown(MOUSE_BUTTON_LEFT)
                                || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)))
        || Keys::anyMove();
    if (userMoved)
        _hasShot = false;
    animate(GetFrameTime());
    applyOrbit();
}

void AppCamera::setShot(Vector3 target, float radius, float pitch)
{
    _hasShot    = true;
    _shotTarget = target;
    _shotRadius = Clamp(radius, MIN_RADIUS, MAX_RADIUS);
    _shotPitch  = pitch;
}

void AppCamera::animate(float dt)
{
    if (_orbitSpeed != 0.0f)
        _yaw += _orbitSpeed * dt;
    if (!_hasShot)
        return;
    // Lissage exponentiel indépendant du framerate (~1,5 s pour arriver).
    float k = 1.0f - expf(-dt * 2.2f);
    _target = Vector3Lerp(_target, _shotTarget, k);
    _radius += (_shotRadius - _radius) * k;
    if (_shotPitch > 0.0f)
        _pitch += (_shotPitch - _pitch) * k;
}

void AppCamera::follow(Vector3 point)
{
    // Recentrage progressif : la cible glisse vers le joueur suivi au lieu
    // de sauter dessus (et accompagne son interpolation de marche). Même
    // lissage qu'à 60 images/s (15 % du chemin par image), à tout framerate.
    float k = 1.0f - powf(1.0f - 0.15f, frameDelta() * 60.0f);
    _target = Vector3Lerp(_target, point, k);
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
