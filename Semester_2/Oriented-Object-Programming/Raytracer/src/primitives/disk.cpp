#include "primitives/disk.hpp"
#include <cmath>

using namespace RayTracer;

double Disk::solvePlaneIntersection(const Math::Vector3D &normal,
    const Math::Vector3D &toPoint, const Math::Vector3D &rayDir)
{
    double denom = normal.dot(rayDir);
    if (std::abs(denom) < 1e-6) return -1.0;
    double t = toPoint.dot(normal) / denom;
    return t > 0.001 ? t : -1.0;
}

Disk::Disk(const Math::Point3D &c, double r, const Math::Vector3D &n,
           const Math::Vector3D &col)
    : center(c), radius(r), normal(n), color(col.x, col.y, col.z, 1.0)
{
    normal = normal * (1.0 / normal.length());
}

Disk::Disk(const Math::Point3D &c, double r, const Math::Vector3D &n,
           const Math::Vector4D &col)
    : center(c), radius(r), normal(n), color(col)
{
    normal = normal * (1.0 / normal.length());
}

HitRecord Disk::hit(const Ray &ray) const
{
    HitRecord record;
    record.hit = false;

    Math::Vector3D rayDir = ray.direction * (1.0 / ray.direction.length());
    double t = solvePlaneIntersection(normal, center - ray.origin, rayDir);
    if (t < 0.0) return record;

    Math::Point3D intersection = ray.origin + rayDir * t;
    if ((intersection - center).length() > radius) return record;

    double denom = normal.dot(rayDir);
    record.hit = true;
    record.t = t;
    record.point = intersection;
    record.normal = (denom > 0) ? normal * -1.0 : normal;
    record.color = color;

    return record;
}
