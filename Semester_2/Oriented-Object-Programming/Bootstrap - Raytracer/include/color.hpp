/*
** EPITECH PROJECT, 2026
** color
** File description:
** color output function for the project bootstrap_raytracer
*/

#pragma once

#include "math.hpp"
#include <iostream>

namespace RayTracer {

    // Write a color pixel to standard output in PPM format.
    // Takes a Vector3D representing RGB components (0.0 to 255.0 range).
    // Converts floating point values to integers in range [0, 255].
    inline void write_color(const Math::Vector3D &color)
    {
        int r = static_cast<int>(color.x);
        int g = static_cast<int>(color.y);
        int b = static_cast<int>(color.z);
        
        // Clamp values to [0, 255] range.
        r = (r < 0) ? 0 : (r > 255) ? 255 : r;
        g = (g < 0) ? 0 : (g > 255) ? 255 : g;
        b = (b < 0) ? 0 : (b > 255) ? 255 : b;
        
        std::cout << r << " " << g << " " << b << "\n";
    }
} /* namespace RayTracer */
