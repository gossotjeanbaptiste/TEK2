/*
** EPITECH PROJECT, 2026
** rectangle3d
** File description:
** rectangle3d class for the project bootstrap_raytracer
*/

#pragma once

#include "math.hpp"

namespace RayTracer {

    // Represents a 3D rectangle (actually a parallelogram) used as a screen for the camera.
    // Maps 2D pixel coordinates (u, v) to 3D world space coordinates.
    class Rectangle3D {
        public:
            Math::Point3D origin;
            Math::Vector3D bottom_side;
            Math::Vector3D left_side;

            // Constructor initializing all attributes.
            Rectangle3D(const Math::Point3D &originValue,
                        const Math::Vector3D &bottomSideValue,
                        const Math::Vector3D &leftSideValue)
                : origin(originValue), bottom_side(bottomSideValue), left_side(leftSideValue)
            {
            }

            // Get the 3D coordinates of a point in the rectangle.
            // Maps normalized 2D coordinates (u, v) in [0, 1] to 3D world space.
            // (0, 0) corresponds to origin (bottom-left corner).
            // (1, 0) corresponds to bottom-right corner.
            // (0, 1) corresponds to top-left corner.
            // Returns: Point3D = origin + u * bottom_side + v * left_side
            Math::Point3D pointAt(double u, double v) const
            {
                return origin + (bottom_side * u) + (left_side * v);
            }
    };
} /* namespace RayTracer */
