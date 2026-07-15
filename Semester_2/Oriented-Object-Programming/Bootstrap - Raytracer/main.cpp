/*
** EPITECH PROJECT, 2026
** main
** File description:
** main of the project bootstrap_raytracer
*/

#include <iostream>
#include "camera.hpp"
#include "sphere.hpp"
#include "color.hpp"

int main(void)
{
    // Image resolution.
    const int width = 400;
    const int height = 400;
    const int max_color = 255;

    // Setup 3D scene.
    // Place the camera screen at z = -1 so rays point toward negative z.
    RayTracer::Camera cam(
        RayTracer::Rectangle3D(
            Math::Point3D(-1.0, -1.0, -1.0),
            Math::Vector3D(2.0, 0.0, 0.0),
            Math::Vector3D(0.0, 2.0, 0.0)
        )
    );

    // Create multiple spheres with different positions and radii.
    RayTracer::Sphere sphere1(Math::Point3D(-0.5, 0.0, 0), 0.35);   // Left sphere (Red)
    RayTracer::Sphere sphere2(Math::Point3D(0, 0.0, 0), 0.35);    // Right sphere (Green)
    RayTracer::Sphere sphere3(Math::Point3D(0.5, 0, 0), 0.4);     // Top sphere (Magenta)
    RayTracer::Sphere sphere4(Math::Point3D(1.0, 0, 0), 0.3);    // Bottom sphere (Yellow)

    // Write PPM header.
    std::cout << "P3\n";
    std::cout << width << " " << height << "\n";
    std::cout << max_color << "\n";

    // Render image: iterate from top to bottom, left to right.
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            // Normalize pixel coordinates to [0, 1] range.
            double u = static_cast<double>(x) / (width - 1);
            double v = static_cast<double>(y) / (height - 1);

            // Cast ray from camera through this pixel.
            RayTracer::Ray ray = cam.ray(u, v);

            // Check intersection with each sphere and assign colors
            Math::Vector3D color;
            if (sphere1.hits(ray)) {
                color = Math::Vector3D(255.0, 0.0, 0.0);     // Red
            } else if (sphere2.hits(ray)) {
                color = Math::Vector3D(0.0, 255.0, 0.0);     // Green
            } else if (sphere3.hits(ray)) {
                color = Math::Vector3D(255.0, 0.0, 255.0);   // Magenta
            } else if (sphere4.hits(ray)) {
                color = Math::Vector3D(255.0, 255.0, 0.0);   // Yellow
            } else {
                color = Math::Vector3D(0.0, 0.0, 255.0);     // Blue background (like original)
            }
            
            RayTracer::write_color(color);
        }
    }

    return 0;
}