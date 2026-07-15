#pragma once

#include "../utility/math/math.hpp"
#include "iprimitive.hpp"
#include <memory>

namespace RayTracer {
    class Transform {
        private:
            Math::Vector3D translation;
            Math::Vector3D rotation; // in degrees (x, y, z)
            Math::Vector3D scale;

        public:
            Transform() : translation(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1) {}

            Transform &translate(const Math::Vector3D &t);

            Transform &rotate(const Math::Vector3D &r);

            Transform &setScale(const Math::Vector3D &s);

            Math::Point3D apply(const Math::Point3D &point) const;

            Math::Vector3D applyRotation(const Math::Vector3D &vec) const;

            Math::Point3D inverse(const Math::Point3D &point) const;

            Math::Vector3D inverseRotation(const Math::Vector3D &vec) const;
            
            Math::Vector3D inverseScale(const Math::Vector3D &vec) const;
            
            bool hasTransform() const {
                return translation.length() > 1e-6 || 
                       rotation.length() > 1e-6 ||
                       scale.x != 1.0 || scale.y != 1.0 || scale.z != 1.0;
            }
    };

    // Decorator pattern: TransformedPrimitive
    class TransformedPrimitive : public IPrimitive {
        private:
            std::shared_ptr<IPrimitive> primitive;
            Transform transform;

        public:
            TransformedPrimitive(std::shared_ptr<IPrimitive> prim, const Transform &t);

            HitRecord hit(const Ray &ray) const override;
    };
}
