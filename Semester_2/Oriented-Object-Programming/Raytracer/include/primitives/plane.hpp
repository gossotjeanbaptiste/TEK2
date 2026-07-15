#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"

namespace RayTracer {
    class Plane : public IPrimitive {
        private:
            static double solvePlaneIntersection(const Math::Vector3D &normal,
                const Math::Vector3D &toPoint, const Math::Vector3D &rayDir);
            
            Math::Point3D point;
            Math::Vector3D normal;
            Math::Vector4D color;
        
        public:
            Plane(const Math::Point3D &p, const Math::Vector3D &n, const Math::Vector3D &col);
            Plane(const Math::Point3D &p, const Math::Vector3D &n, const Math::Vector4D &col);

            HitRecord hit(const Ray &ray) const override;
    };
}
