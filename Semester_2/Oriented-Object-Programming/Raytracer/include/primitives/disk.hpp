#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"

namespace RayTracer {
    class Disk : public IPrimitive {
        private:
            static double solvePlaneIntersection(const Math::Vector3D &normal,
                const Math::Vector3D &toPoint, const Math::Vector3D &rayDir);
            
            Math::Point3D center;
            double radius;
            Math::Vector3D normal;
            Math::Vector4D color;

        public:
            Disk(const Math::Point3D &c, double r, const Math::Vector3D &n,
                 const Math::Vector3D &col);
            Disk(const Math::Point3D &c, double r, const Math::Vector3D &n,
                 const Math::Vector4D &col);

            HitRecord hit(const Ray &ray) const override;
    };
}
