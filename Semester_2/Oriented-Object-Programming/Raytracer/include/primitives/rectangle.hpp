#pragma once

#include "../utility/math/math.hpp"

namespace RayTracer {
    class Rectangle3D {
        public:
            Math::Point3D origin;
            Math::Vector3D bottom_side;
            Math::Vector3D left_side;

            Rectangle3D(const Math::Point3D &originValue, const Math::Vector3D &bottomSideValue, const Math::Vector3D &leftSideValue) : origin(originValue), bottom_side(bottomSideValue), left_side(leftSideValue)
            {
            }

            Math::Point3D pointAt(double u, double v) const
            {
                return origin + (bottom_side * u) + (left_side * v);
            }
    };
}
