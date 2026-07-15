#include "../../include/lights/light.hpp"

using namespace RayTracer;

AmbientLight::AmbientLight(double intens, const Math::Vector3D &col)
    : intensity(intens), color(col)
{
}

Math::Vector3D AmbientLight::getIntensity(const Math::Point3D &) const
{
    return Math::Vector3D(
        color.x * intensity,
        color.y * intensity,
        color.z * intensity
    );
}

Math::Vector3D AmbientLight::getDirection(const Math::Point3D &) const
{
    return Math::Vector3D(0, 0, 0);
}