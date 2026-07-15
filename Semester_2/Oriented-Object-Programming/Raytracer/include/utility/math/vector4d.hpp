#pragma once
#include <cmath>

namespace Math {
    class Vector4D {
        public:
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
            double w = 1.0;  // w = alpha (0.0 = transparent, 1.0 = opaque)

            Vector4D() = default;
            Vector4D(double xValue, double yValue, double zValue, double wValue = 1.0)
                : x(xValue), y(yValue), z(zValue), w(wValue) { }

            double length() const
            {
                return std::sqrt((x * x) + (y * y) + (z * z));
            }

            double dot(const Vector4D &other) const
            {
                return (x * other.x) + (y * other.y) + (z * other.z);
            }

            Vector4D operator+(const Vector4D &other) const
            {
                return {x + other.x, y + other.y, z + other.z, w};
            }

            Vector4D &operator+=(const Vector4D &other)
            {
                x += other.x;
                y += other.y;
                z += other.z;
                return *this;
            }

            Vector4D operator-(const Vector4D &other) const
            {
                return {x - other.x, y - other.y, z - other.z, w};
            }

            Vector4D &operator-=(const Vector4D &other)
            {
                x -= other.x;
                y -= other.y;
                z -= other.z;
                return *this;
            }

            Vector4D operator*(const Vector4D &other) const
            {
                return {x * other.x, y * other.y, z * other.z, w};
            }

            Vector4D &operator*=(const Vector4D &other)
            {
                x *= other.x;
                y *= other.y;
                z *= other.z;
                return *this;
            }

            Vector4D operator/(const Vector4D &other) const
            {
                return {x / other.x, y / other.y, z / other.z, w};
            }

            Vector4D &operator/=(const Vector4D &other)
            {
                x /= other.x;
                y /= other.y;
                z /= other.z;
                return *this;
            }

            Vector4D operator*(double value) const
            {
                return {x * value, y * value, z * value, w};
            }

            Vector4D &operator*=(double value)
            {
                x *= value;
                y *= value;
                z *= value;
                return *this;
            }

            Vector4D operator/(double value) const
            {
                return {x / value, y / value, z / value, w};
            }

            Vector4D &operator/=(double value)
            {
                x /= value;
                y /= value;
                z /= value;
                return *this;
            }

            Vector4D operator-() const
            {
                return {-x, -y, -z, w};
            }

            Vector4D normalize() const
            {
                double len = length();
                if (len == 0.0) return *this;
                return {x / len, y / len, z / len, w};
            }
    };
}
