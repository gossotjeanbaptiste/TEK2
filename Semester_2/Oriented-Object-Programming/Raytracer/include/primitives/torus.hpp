#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"
#include "../utility/ray.hpp"

namespace RayTracer {
    class Torus : public IPrimitive {
        private:
            Math::Point3D center;
            double majorRadius; // Grand rayon (R) : distance du centre au milieu du tube
            double minorRadius; // Petit rayon (r) : rayon du tube lui-même
            Math::Vector4D color;

        public:
            Torus(const Math::Point3D &centerValue, double majorR, double minorR, const Math::Vector3D &col);
            
            Torus(const Math::Point3D &centerValue, double majorR, double minorR, const Math::Vector4D &col);

            ~Torus() override = default;

            HitRecord hit(const Ray &ray) const override;
    };
}
