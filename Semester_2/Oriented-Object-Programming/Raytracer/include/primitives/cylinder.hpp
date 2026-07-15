#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"

namespace RayTracer {
    class Cylinder : public IPrimitive {
        private:
            static double solveCylinderQuadratic(double a, double b, double c);
            
            Math::Point3D center;
            double radius;
            Math::Vector3D axis;
            Math::Vector4D color;
        
        public:
            Cylinder(const Math::Point3D &c, double r, 
                     const Math::Vector3D &ax, const Math::Vector3D &col);
            Cylinder(const Math::Point3D &c, double r, 
                     const Math::Vector3D &ax, const Math::Vector4D &col);

            HitRecord hit(const Ray &ray) const override;
    };
}
