#pragma once

#include "math/math.hpp"
#include <iostream>
#include <fstream>

namespace RayTracer {
    inline void write_color(std::ostream &file, const Math::Vector3D &color)
    {
        int r = static_cast<int>(color.x);
        int g = static_cast<int>(color.y);
        int b = static_cast<int>(color.z);

        r = (r < 0) ? 0 : (r > 255) ? 255 : r;
        g = (g < 0) ? 0 : (g > 255) ? 255 : g;
        b = (b < 0) ? 0 : (b > 255) ? 255 : b;

        file << r << " " << g << " " << b << "\n";
    }

    inline void write_color(const Math::Vector3D &color)
    {
        write_color(std::cout, color);
    }

    // Support pour Vector4D avec alpha
    inline void write_color(std::ostream &file, const Math::Vector4D &color)
    {
        int r = static_cast<int>(color.x);
        int g = static_cast<int>(color.y);
        int b = static_cast<int>(color.z);
        int a = static_cast<int>(color.w * 255.0);

        r = (r < 0) ? 0 : (r > 255) ? 255 : r;
        g = (g < 0) ? 0 : (g > 255) ? 255 : g;
        b = (b < 0) ? 0 : (b > 255) ? 255 : b;
        a = (a < 0) ? 0 : (a > 255) ? 255 : a;

        file << r << " " << g << " " << b << " " << a << "\n";
    }

    inline void write_color(const Math::Vector4D &color)
    {
        write_color(std::cout, color);
    }
}
