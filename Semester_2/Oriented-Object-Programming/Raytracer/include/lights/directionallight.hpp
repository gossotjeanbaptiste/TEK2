#pragma once

#include "../utility/math/math.hpp"
#include "ILight.hpp"

namespace RayTracer {
    class DirectionalLight : public ILight {
        private:
            Math::Vector3D direction;
            Math::Vector3D color;
            double intensity;
        public:
            DirectionalLight(const Math::Vector3D &dir, const Math::Vector3D &col, double intens);
            Math::Vector3D getIntensity(const Math::Point3D &) const override;
            Math::Vector3D getDirection(const Math::Point3D &) const override;
    };
}
