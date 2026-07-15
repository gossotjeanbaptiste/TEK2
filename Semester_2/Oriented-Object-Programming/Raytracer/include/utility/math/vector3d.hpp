
#pragma once
#include <cmath>

namespace Math {
    class Vector3D {
        public:
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            Vector3D() = default;
            Vector3D(double xValue, double yValue, double zValue) : x(xValue), y(yValue), z(zValue)
            {
            }

            double length() const
            {
                return std::sqrt((x * x) + (y * y) + (z * z));
            }

            double dot(const Vector3D &other) const
            {
                return (x * other.x) + (y * other.y) + (z * other.z);
            }

            Vector3D operator+(const Vector3D &other) const
            {
                return {x + other.x, y + other.y, z + other.z};
            }

            Vector3D &operator+=(const Vector3D &other)
            {
                x += other.x;
                y += other.y;
                z += other.z;
                return *this;
            }

            Vector3D operator-(const Vector3D &other) const
            {
                return {x - other.x, y - other.y, z - other.z};
            }

            Vector3D &operator-=(const Vector3D &other)
            {
                x -= other.x;
                y -= other.y;
                z -= other.z;
                return *this;
            }

            Vector3D operator*(const Vector3D &other) const
            {
                return {x * other.x, y * other.y, z * other.z};
            }

            Vector3D &operator*=(const Vector3D &other)
            {
                x *= other.x;
                y *= other.y;
                z *= other.z;
                return *this;
            }

            Vector3D operator/(const Vector3D &other) const
            {
                return {x / other.x, y / other.y, z / other.z};
            }

            Vector3D &operator/=(const Vector3D &other)
            {
                x /= other.x;
                y /= other.y;
                z /= other.z;
                return *this;
            }

            Vector3D operator*(double value) const
            {
                return {x * value, y * value, z * value};
            }

            Vector3D &operator*=(double value)
            {
                x *= value;
                y *= value;
                z *= value;
                return *this;
            }

            Vector3D operator/(double value) const
            {
                return {x / value, y / value, z / value};
            }

            Vector3D &operator/=(double value)
            {
                x /= value;
                y /= value;
                z /= value;
                return *this;
            }

            Vector3D translate(const Vector3D &displacement) const
            {
                return {x + displacement.x, y + displacement.y, z + displacement.z};
            }

            Vector3D rotateX(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x, y * cos_a - z * sin_a, y * sin_a + z * cos_a};
            }

            Vector3D rotateY(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a + z * sin_a, y, -x * sin_a + z * cos_a};
            }

            Vector3D rotateZ(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a - y * sin_a, x * sin_a + y * cos_a, z};
            }
    };

    inline Vector3D operator*(double value, const Vector3D &vector)
    {
        return vector * value;
    }
}