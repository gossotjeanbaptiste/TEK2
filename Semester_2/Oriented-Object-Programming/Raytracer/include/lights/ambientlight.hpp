#pragma once

#include "../utility/math/math.hpp"
#include "ILight.hpp"

namespace RayTracer {
    class AmbientLight : public ILight {
        private:
            double intensity;
            Math::Vector3D color;
        public:
            AmbientLight(double intens, const Math::Vector3D &col);
            Math::Vector3D getIntensity(const Math::Point3D &) const override;
            Math::Vector3D getDirection(const Math::Point3D &) const override;
    };
}