/*
** EPITECH PROJECT, 2026
** sphere
** File description:
** sphere class for the project bootstrap_raytracer
*/

#pragma once

#include "math.hpp"
#include "ray.hpp"

namespace RayTracer {

    // Represents a sphere in 3D space.
    // Used to test ray-sphere intersection via quadratic equation solving.
    class Sphere {
        public:
            Math::Point3D center;
            double radius;

            // Constructor initializing the center and radius of the sphere.
            Sphere(const Math::Point3D &centerValue, double radiusValue)
                : center(centerValue), radius(radiusValue)
            {
            }

            // Check if a ray intersects with this sphere.
            // Uses parametric equations and the quadratic formula.
            // Returns true if there exists at least one intersection point (discriminant >= 0).
            bool hits(const Ray &ray) const
            {
                // Vector from sphere center to ray origin: O - C
                Math::Vector3D oc(ray.origin.x - center.x, ray.origin.y - center.y, ray.origin.z - center.z);

                // Quadratic equation coefficients for: a*k² + b*k + c = 0
                // where k is the parameter along the ray P = O + D*k
                double a = ray.direction.dot(ray.direction);
                double b = 2.0 * oc.dot(ray.direction);
                double c = oc.dot(oc) - (radius * radius);

                // Discriminant: if >= 0, the ray intersects the sphere
                double discriminant = (b * b) - (4.0 * a * c);

                return discriminant >= 0.0;
            }
    };
} /* namespace RayTracer */
