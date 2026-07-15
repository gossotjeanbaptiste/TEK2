#include "primitives/limited_cylinder.hpp"
#include <cmath>

using namespace RayTracer;

double LimitedCylinder::solveCylinderQuadratic(double a, double b, double c)
{
    double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0 || a < 1e-6) return -1.0;
    
    double t1 = (-b - std::sqrt(discriminant)) / (2.0 * a);
    double t2 = (-b + std::sqrt(discriminant)) / (2.0 * a);
    double t = (t1 > 0.001) ? t1 : t2;
    return t > 0.001 ? t : -1.0;
}

LimitedCylinder::LimitedCylinder(const Math::Point3D &c, double r, double h, 
                    const Math::Vector3D &ax, const Math::Vector3D &col)
    : center(c), radius(r), height(h), axis(ax), color(col.x, col.y, col.z, 1.0)
{
    axis = axis * (1.0 / axis.length());
}

LimitedCylinder::LimitedCylinder(const Math::Point3D &c, double r, double h, 
                    const Math::Vector3D &ax, const Math::Vector4D &col)
    : center(c), radius(r), height(h), axis(ax), color(col)
{
    axis = axis * (1.0 / axis.length());
}

HitRecord LimitedCylinder::hit(const Ray &ray) const
{
    HitRecord record;
    record.hit = false;

    Math::Vector3D oc = ray.origin - center;
    Math::Vector3D rayDir = ray.direction * (1.0 / ray.direction.length());

    Math::Vector3D w = axis * axis.dot(rayDir);
    Math::Vector3D rayPerpendicular = rayDir - w;
    Math::Vector3D ocPerpendicular = oc - axis * axis.dot(oc);

    double a = rayPerpendicular.dot(rayPerpendicular);
    double b = 2.0 * ocPerpendicular.dot(rayPerpendicular);
    double c = ocPerpendicular.dot(ocPerpendicular) - (radius * radius);

    double t = solveCylinderQuadratic(a, b, c);
    if (t < 0.0) return record;

    Math::Point3D intersection = ray.origin + rayDir * t;
    double heightOnAxis = (intersection - center).dot(axis);
    
    if (heightOnAxis < 0.0 || heightOnAxis > height) return record;

    record.hit = true;
    record.t = t;
    record.point = intersection;
    Math::Vector3D normalVec = (intersection - center) - axis * heightOnAxis;
    record.normal = normalVec * (1.0 / normalVec.length());
    record.color = color;

    return record;
}
