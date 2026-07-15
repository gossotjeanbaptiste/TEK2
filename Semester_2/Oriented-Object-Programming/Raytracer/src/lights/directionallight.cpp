#include "../../include/lights/light.hpp"

using namespace RayTracer;

DirectionalLight::DirectionalLight(const Math::Vector3D &dir, const Math::Vector3D &col, double intens)
    : direction(dir), color(col), intensity(intens)
{
    direction = direction * (1.0 / direction.length());
}

Math::Vector3D DirectionalLight::getIntensity(const Math::Point3D &) const
{
    return Math::Vector3D(
        color.x * intensity,
        color.y * intensity,
        color.z * intensity
    );
}

Math::Vector3D DirectionalLight::getDirection(const Math::Point3D &) const
{
    return direction;
}
