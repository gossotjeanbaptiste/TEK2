#pragma once

#include "../utility/math/math.hpp"

namespace RayTracer {
    class ILight {
        public:
            virtual ~ILight() = default;
            virtual Math::Vector3D getIntensity(const Math::Point3D &point) const = 0;
            virtual Math::Vector3D getDirection(const Math::Point3D &point) const = 0;
    };
}
