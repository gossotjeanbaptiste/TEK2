#pragma once

#include "math/math.hpp"
#include "../primitives/rectangle.hpp"

namespace RayTracer {
    class Camera {
        public:
            Math::Point3D origin;
            Rectangle3D screen;

            Camera() : origin(0.0, 0.0, 0.0),screen(Math::Point3D(0.0, 0.0, 0.0), Math::Vector3D(1.0, 0.0, 0.0), Math::Vector3D(0.0, 1.0, 0.0))
            {
            }

            Camera(const Rectangle3D &screenValue) : origin(0.0, 0.0, 0.0), screen(screenValue)
            {
            }

            Camera(const Camera &) = default;

            Camera &operator=(const Camera &) = default;

            Camera(Camera &&) = default;

            Camera &operator=(Camera &&) = default;

            ~Camera() = default;

            Ray ray(double u, double v) const
            {
                Math::Point3D pixelPos = screen.pointAt(u, v);
                Math::Vector3D direction(pixelPos.x - origin.x, pixelPos.y - origin.y, pixelPos.z - origin.z);
                direction = direction / direction.length();
                return Ray(origin, direction);
            }
    };
}
