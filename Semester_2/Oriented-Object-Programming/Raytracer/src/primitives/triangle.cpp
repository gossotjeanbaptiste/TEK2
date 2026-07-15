#include "primitives/triangle.hpp"
#include <cmath>

using namespace RayTracer;

Math::Vector3D Triangle::crossProduct(const Math::Vector3D &a, const Math::Vector3D &b)
{
    return Math::Vector3D(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

Triangle::Triangle(const Math::Point3D &vertex0, const Math::Point3D &vertex1,
    const Math::Point3D &vertex2, const Math::Vector3D &col)
    : p0(vertex0), p1(vertex1), p2(vertex2), color(col.x, col.y, col.z, 1.0)
{
    Math::Vector3D edge1 = p1 - p0;
    Math::Vector3D edge2 = p2 - p0;
    
    normal = crossProduct(edge1, edge2);
    
    double length = normal.length();
    if (length > 1e-6) {
        normal = normal * (1.0 / length);
    }
}

Triangle::Triangle(const Math::Point3D &vertex0, const Math::Point3D &vertex1,
    const Math::Point3D &vertex2, const Math::Vector4D &col)
    : p0(vertex0), p1(vertex1), p2(vertex2), color(col)
{
    Math::Vector3D edge1 = p1 - p0;
    Math::Vector3D edge2 = p2 - p0;
    
    normal = crossProduct(edge1, edge2);
    
    double length = normal.length();
    if (length > 1e-6) {
        normal = normal * (1.0 / length);
    }
}

HitRecord Triangle::hit(const Ray &ray) const
{
    HitRecord record;
    record.hit = false;
    const double EPSILON = 1e-10;
    
    Math::Vector3D edge1 = p1 - p0;
    Math::Vector3D edge2 = p2 - p0;
    Math::Vector3D h = crossProduct(ray.direction, edge2);
    
    double a = edge1.dot(h);
    if (std::abs(a) < EPSILON) return record;
    
    double f = 1.0 / a;
    Math::Vector3D s = ray.origin - p0;
    double u = f * s.dot(h);
    if (u < 0.0 || u > 1.0) return record;
    
    Math::Vector3D q = crossProduct(s, edge1);
    double v = f * ray.direction.dot(q);
    if (v < 0.0 || u + v > 1.0) return record;
    
    double t = f * edge2.dot(q);
    if (t > EPSILON) {
        record.hit = true;
        record.t = t;
        record.point = ray.origin + ray.direction * t;
        record.normal = normal;
        record.color = color;
    }
    
    return record;
}