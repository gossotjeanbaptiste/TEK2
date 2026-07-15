#include "primitives/limited_cone.hpp"
#include <cmath>

using namespace RayTracer;

double LimitedCone::solveConeQuadratic(double a, double b, double c)
{
    double discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0 || std::abs(a) < 1e-6) return -1.0;
    
    double t1 = (-b - std::sqrt(discriminant)) / (2.0 * a);
    double t2 = (-b + std::sqrt(discriminant)) / (2.0 * a);
    double t = (t1 > 0.001) ? t1 : t2;
    return t > 0.001 ? t : -1.0;
}

LimitedCone::LimitedCone(const Math::Point3D &a, double h, double br,
            const Math::Vector3D &ax, const Math::Vector3D &col)
    : apex(a), height(h), baseRadius(br), axis(ax), color(col.x, col.y, col.z, 1.0)
{
    axis = axis * (1.0 / axis.length());
}

LimitedCone::LimitedCone(const Math::Point3D &a, double h, double br,
            const Math::Vector3D &ax, const Math::Vector4D &col)
    : apex(a), height(h), baseRadius(br), axis(ax), color(col)
{
    axis = axis * (1.0 / axis.length());
}

HitRecord LimitedCone::hit(const Ray &ray) const
{
    HitRecord record;
    record.hit = false;

    Math::Vector3D oc = ray.origin - apex;
    Math::Vector3D rayDir = ray.direction * (1.0 / ray.direction.length());

    double cosTheta = height / std::sqrt(baseRadius * baseRadius + height * height);
    double cosThetaSq = cosTheta * cosTheta;

    double dotOA = oc.dot(axis);
    double dotDA = rayDir.dot(axis);

    double a = dotDA * dotDA - cosThetaSq;
    double b = 2.0 * (dotOA * dotDA - cosThetaSq * oc.dot(rayDir));
    double c = dotOA * dotOA - cosThetaSq * oc.dot(oc);

    double t = solveConeQuadratic(a, b, c);
    if (t < 0.0) return record;

    Math::Vector3D toApex = ray.origin + rayDir * t - apex;
    double distOnAxis = toApex.dot(axis);
    
    if (distOnAxis < 0.0 || distOnAxis > height) return record;

    record.hit = true;
    record.t = t;
    record.point = ray.origin + rayDir * t;
    
    Math::Vector3D normalVec = axis * distOnAxis - toApex * cosThetaSq;
    record.normal = normalVec.length() < 1e-6 ? axis : normalVec * (1.0 / normalVec.length());
    record.color = color;

    return record;
}
