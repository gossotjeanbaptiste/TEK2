#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"

namespace RayTracer {
    class LimitedCylinder : public IPrimitive {
        private:
            static double solveCylinderQuadratic(double a, double b, double c);
            
            Math::Point3D center;
            double radius;
            double height;
            Math::Vector3D axis;
            Math::Vector4D color;

        public:
            LimitedCylinder(const Math::Point3D &c, double r, double h, 
                     const Math::Vector3D &ax, const Math::Vector3D &col);
            LimitedCylinder(const Math::Point3D &c, double r, double h, 
                     const Math::Vector3D &ax, const Math::Vector4D &col);

            HitRecord hit(const Ray &ray) const override;
    };
}
