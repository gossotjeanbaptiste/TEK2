#include "primitives/plane.hpp"
#include <cmath>

using namespace RayTracer;

double Plane::solvePlaneIntersection(const Math::Vector3D &normal,
    const Math::Vector3D &toPoint, const Math::Vector3D &rayDir)
{
    double denom = normal.dot(rayDir);
    if (std::abs(denom) < 1e-6) return -1.0;
    double t = toPoint.dot(normal) / denom;
    return t > 0.001 ? t : -1.0;
}

Plane::Plane(const Math::Point3D &p, const Math::Vector3D &n,
    const Math::Vector3D &col)
    : point(p), normal(n), color(col.x, col.y, col.z, 1.0)
{
    normal = normal * (1.0 / normal.length());
}

Plane::Plane(const Math::Point3D &p, const Math::Vector3D &n,
    const Math::Vector4D &col)
    : point(p), normal(n), color(col)
{
    normal = normal * (1.0 / normal.length());
}

HitRecord Plane::hit(const Ray &ray) const
{
    HitRecord record;
    record.hit = false;

    double t = solvePlaneIntersection(normal, point - ray.origin, ray.direction);
    if (t < 0.0) return record;

    double denom = normal.dot(ray.direction);
    record.hit = true;
    record.t = t;
    record.point = ray.origin + ray.direction * t;
    record.normal = (denom > 0) ? normal * -1.0 : normal;
    record.color = color;

    return record;
}

