/*
** EPITECH PROJECT, 2026
** math
** File description:
** math functions for the project bootstrap_raytracer
*/


#pragma once

#include <cmath>

namespace Math {

    // Represents a 3D vector used for directions, colors, and geometric computations.
    // Vectors support addition, subtraction, scaling, and component-wise operations.
    class Vector3D {
        public:
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            // Default constructor initializes vector to zero.
            Vector3D() = default;
            // Construct a vector with given x, y, z components.
            Vector3D(double xValue, double yValue, double zValue) : x(xValue), y(yValue), z(zValue)
            {
            }

            // Calculate the Euclidean norm (magnitude) of the vector.
            // Used for normalization and distance calculations in raytracing.
            double length() const
            {
                return std::sqrt((x * x) + (y * y) + (z * z));
            }

            // Calculate scalar (dot) product with another vector.
            // Used for angle calculations, lighting, and surface normal computations.
            double dot(const Vector3D &other) const
            {
                return (x * other.x) + (y * other.y) + (z * other.z);
            }

            // Add two vectors component-wise (element-by-element).
            Vector3D operator+(const Vector3D &other) const
            {
                return {x + other.x, y + other.y, z + other.z};
            }

            // Add another vector to this vector in-place.
            Vector3D &operator+=(const Vector3D &other)
            {
                x += other.x;
                y += other.y;
                z += other.z;
                return *this;
            }

            // Subtract another vector component-wise.
            Vector3D operator-(const Vector3D &other) const
            {
                return {x - other.x, y - other.y, z - other.z};
            }

            // Subtract another vector from this vector in-place.
            Vector3D &operator-=(const Vector3D &other)
            {
                x -= other.x;
                y -= other.y;
                z -= other.z;
                return *this;
            }

            // Element-wise multiplication (Hadamard product).
            // Useful for color channel multiplication in raytracing.
            Vector3D operator*(const Vector3D &other) const
            {
                return {x * other.x, y * other.y, z * other.z};
            }

            // Element-wise multiplication in-place.
            Vector3D &operator*=(const Vector3D &other)
            {
                x *= other.x;
                y *= other.y;
                z *= other.z;
                return *this;
            }

            // Element-wise division (Hadamard division).
            Vector3D operator/(const Vector3D &other) const
            {
                return {x / other.x, y / other.y, z / other.z};
            }

            // Element-wise division in-place.
            Vector3D &operator/=(const Vector3D &other)
            {
                x /= other.x;
                y /= other.y;
                z /= other.z;
                return *this;
            }

            // Scalar operations: multiply each component by a scalar value.
            // Used for scaling vectors (e.g., attenuating colors, ray direction scaling).
            Vector3D operator*(double value) const
            {
                return {x * value, y * value, z * value};
            }

            // Scalar multiplication in-place.
            Vector3D &operator*=(double value)
            {
                x *= value;
                y *= value;
                z *= value;
                return *this;
            }

            // Scalar division: divide each component by a scalar value.
            Vector3D operator/(double value) const
            {
                return {x / value, y / value, z / value};
            }

            // Scalar division in-place.
            Vector3D &operator/=(double value)
            {
                x /= value;
                y /= value;
                z /= value;
                return *this;
            }

            // Translate (move) the vector by adding another vector's displacement.
            // Returns a new vector at the translated location.
            Vector3D translate(const Vector3D &displacement) const
            {
                return {x + displacement.x, y + displacement.y, z + displacement.z};
            }

            // Rotate the vector around the X axis by a given angle in degrees.
            // Returns a new rotated vector.
            Vector3D rotateX(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x, y * cos_a - z * sin_a, y * sin_a + z * cos_a};
            }

            // Rotate the vector around the Y axis by a given angle in degrees.
            // Returns a new rotated vector.
            Vector3D rotateY(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a + z * sin_a, y, -x * sin_a + z * cos_a};
            }

            // Rotate the vector around the Z axis by a given angle in degrees.
            // Returns a new rotated vector.
            Vector3D rotateZ(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a - y * sin_a, x * sin_a + y * cos_a, z};
            }
    };

    // Global operator for scalar-first multiplication (scalar * vector).
    // Allows commutative scalar multiplication: 2.0 * vec is equivalent to vec * 2.0
    inline Vector3D operator*(double value, const Vector3D &vector)
    {
        return vector * value;
    }

    // Represents a point in 3D space (different from vectors conceptually).
    // Points are absolute positions, while vectors represent directions and displacements.
    class Point3D {
        public:
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            // Default constructor initializes point to origin (0, 0, 0).
            Point3D() = default;
            // Construct a point at the specified coordinates.
            Point3D(double xValue, double yValue, double zValue) : x(xValue), y(yValue), z(zValue)
            {
            }

            // Translate this point by a vector, resulting in a new point.
            // Mathematical operation: Point + Vector = Point
            Point3D operator+(const Vector3D &vector) const
            {
                return {x + vector.x, y + vector.y, z + vector.z};
            }

            // Translate this point in-place by adding a vector's components.
            Point3D &operator+=(const Vector3D &vector)
            {
                x += vector.x;
                y += vector.y;
                z += vector.z;
                return *this;
            }

            // Translate (move) the point by a vector displacement.
            // Returns a new point at the translated location.
            Point3D translate(const Vector3D &displacement) const
            {
                return {x + displacement.x, y + displacement.y, z + displacement.z};
            }

            // Rotate the point around the X axis by a given angle in degrees.
            // Rotation is performed around the origin (0, 0, 0).
            Point3D rotateX(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x, y * cos_a - z * sin_a, y * sin_a + z * cos_a};
            }

            // Rotate the point around the Y axis by a given angle in degrees.
            // Rotation is performed around the origin (0, 0, 0).
            Point3D rotateY(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a + z * sin_a, y, -x * sin_a + z * cos_a};
            }

            // Rotate the point around the Z axis by a given angle in degrees.
            // Rotation is performed around the origin (0, 0, 0).
            Point3D rotateZ(double degrees) const
            {
                double radians = degrees * M_PI / 180.0;
                double cos_a = std::cos(radians);
                double sin_a = std::sin(radians);
                return {x * cos_a - y * sin_a, x * sin_a + y * cos_a, z};
            }
    };
} /* namespace Math */