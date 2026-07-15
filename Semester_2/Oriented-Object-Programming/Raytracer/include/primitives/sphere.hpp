#pragma once

#include "iprimitive.hpp"
#include "../utility/math/math.hpp"
#include "../utility/ray.hpp"

namespace RayTracer {
    class Sphere : public IPrimitive {
        private:
            static double solveSphereIntersection(double a, double b, double c);
            
            Math::Point3D center;
            double radius;
            Math::Vector4D color;

        public:
            // Constructeur avec Vector3D (alpha = 1.0 par défaut)
            Sphere(const Math::Point3D &centerValue, double radiusValue, const Math::Vector3D &col);
            // Constructeur avec Vector4D (avec transparence)
            Sphere(const Math::Point3D &centerValue, double radiusValue, const Math::Vector4D &col);

            HitRecord hit(const Ray &ray) const override;

            // Pour compatibilité
            bool hits(const Ray &ray) const;
    };
}
