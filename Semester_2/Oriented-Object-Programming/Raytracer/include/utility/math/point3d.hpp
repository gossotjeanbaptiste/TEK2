#pragma once
#include "vector3d.hpp"
#include <cmath>

namespace Math {
    class Point3D {
        public:
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            Point3D() = default;
            Point3D(double xValue, double yValue, double zValue) : x(xValue), y(yValue), z(zValue)
            {
            }

            Point3D operator+(const Vector3D &vector) const
            {
                return {x + vector.x, y + vector.y, z + vector.z};
            }

            Point3D &operator+=(const Vector3D &vector)
            {
                x += vector.x;
                y += vector.y;
                z += vector.z;
                return *this;
            }

            Vector3D operator-(const Point3D &other) const
            {
                return {x - other.x, y - other.y, z - other.z};
            }

            Point3D operator-(const Vector3D &vector) const
            {
                return {x - vector.x, y - vector.y, z - vector.z};
            }

            Point3D &operator-=(const Vector3D &vector)
            {
                x -= vector.x;
                y -= vector.y;
                z -= vector.z;
                return *this;
            }

            Point3D translate(const Vector3D &displacement) const
            {
                return {x + displacement.x, y + displacement.y, z + displacement.z};
            }

            Point3D rotateX(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x, y * cos_a - z * sin_a, y * sin_a + z * cos_a};
            }

            Point3D rotateY(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a + z * sin_a, y, -x * sin_a + z * cos_a};
            }

            Point3D rotateZ(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a - y * sin_a, x * sin_a + y * cos_a, z};
            }
    };
}