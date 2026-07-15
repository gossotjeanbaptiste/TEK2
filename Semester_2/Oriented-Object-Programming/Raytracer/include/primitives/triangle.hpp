#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"
#include "../utility/ray.hpp"

namespace RayTracer {
    class Triangle : public IPrimitive {
        private:
            static Math::Vector3D crossProduct(const Math::Vector3D &a, const Math::Vector3D &b);
            
            Math::Point3D p0;
            Math::Point3D p1;
            Math::Point3D p2;
            Math::Vector4D color;
            Math::Vector3D normal;

        public:
            Triangle(const Math::Point3D &vertex0, const Math::Point3D &vertex1,
                     const Math::Point3D &vertex2, const Math::Vector3D &col);
            Triangle(const Math::Point3D &vertex0, const Math::Point3D &vertex1,
                     const Math::Point3D &vertex2, const Math::Vector4D &col);

            HitRecord hit(const Ray &ray) const override;
    };
}