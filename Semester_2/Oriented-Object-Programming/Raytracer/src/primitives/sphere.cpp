#include "primitives/sphere.hpp"
#include <cmath>

using namespace RayTracer;

Sphere::Sphere(const Math::Point3D &centerValue, 
    double radiusValue, const Math::Vector3D &col)
    : center(centerValue), radius(radiusValue), color(col.x, col.y, col.z, 1.0)
{
}

Sphere::Sphere(const Math::Point3D &centerValue, 
    double radiusValue, const Math::Vector4D &col)
    : center(centerValue), radius(radiusValue), color(col)
{
}

double Sphere::solveSphereIntersection(double a, double b, double c)
{
    double discriminant = (b * b) - (4.0 * a * c);
    if (discriminant < 0.0) return -1.0;
    
    double t = (-b - std::sqrt(discriminant)) / (2.0 * a);
    if (t < 0.001) {
        t = (-b + std::sqrt(discriminant)) / (2.0 * a);
    }
    return t > 0.001 ? t : -1.0;
}

HitRecord Sphere::hit(const Ray &ray) const
{
    HitRecord record;
    record.hit = false;

    Math::Vector3D oc = ray.origin - center;
    double a = ray.direction.dot(ray.direction);
    double b = 2.0 * oc.dot(ray.direction);
    double c = oc.dot(oc) - (radius * radius);

    double t = solveSphereIntersection(a, b, c);
    if (t < 0.0) return record;

    record.hit = true;
    record.t = t;
    record.point = ray.origin + ray.direction * t;
    Math::Vector3D normal = record.point - center;
    record.normal = normal / normal.length();
    record.color = color;

    return record;
}

bool Sphere::hits(const Ray &ray) const
{
    return hit(ray).hit;
}

