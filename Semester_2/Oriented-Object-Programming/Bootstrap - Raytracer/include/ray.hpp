/*
** EPITECH PROJECT, 2026
** ray
** File description:
** ray class for the project bootstrap_raytracer
*/

#pragma once

#include "math.hpp"

namespace RayTracer {

    class Ray {
        public:
            Math::Point3D origin;
            Math::Vector3D direction;

            Ray() = default;
            Ray(const Math::Point3D &originValue, const Math::Vector3D &directionValue)
                : origin(originValue), direction(directionValue)
            {
            }
    };
} /* namespace RayTracer */