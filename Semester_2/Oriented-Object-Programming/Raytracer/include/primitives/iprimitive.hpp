#pragma once

#include "../utility/math/math.hpp"
#include "../utility/ray.hpp"

namespace RayTracer {
    struct HitRecord {
        double t;
        Math::Point3D point;
        Math::Vector3D normal;
        Math::Vector4D color;  // Stocke RGBA (w = alpha)
        bool hit;
    };

    class IPrimitive {
        public:
            virtual ~IPrimitive() = default;
            virtual HitRecord hit(const Ray &ray) const = 0;
    };
}
