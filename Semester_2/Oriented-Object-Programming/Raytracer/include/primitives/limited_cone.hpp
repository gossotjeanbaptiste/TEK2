#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"

namespace RayTracer {
    class LimitedCone : public IPrimitive {
        private:
            static double solveConeQuadratic(double a, double b, double c);
            
            Math::Point3D apex;
            double height;
            double baseRadius;
            Math::Vector3D axis;
            Math::Vector4D color;

        public:
            LimitedCone(const Math::Point3D &a, double h, double br,
                         const Math::Vector3D &ax, const Math::Vector3D &col);
            LimitedCone(const Math::Point3D &a, double h, double br,
                         const Math::Vector3D &ax, const Math::Vector4D &col);

            HitRecord hit(const Ray &ray) const override;
    };
}
