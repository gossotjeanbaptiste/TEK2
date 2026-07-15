#include "primitives/transform.hpp"

using namespace RayTracer;

Transform &Transform::translate(const Math::Vector3D &t)
{
    translation += t;
    return *this;
}

Transform &Transform::rotate(const Math::Vector3D &r)
{
    rotation += r;
    return *this;
}

Transform &Transform::setScale(const Math::Vector3D &s)
{
    scale = s;
    return *this;
}

Math::Point3D Transform::apply(const Math::Point3D &point) const
{
    Math::Vector3D vec(point.x, point.y, point.z);
    
    // Scale
    vec = Math::Vector3D(vec.x * scale.x, vec.y * scale.y, vec.z * scale.z);
    
    // Rotate
    vec = vec.rotateX(rotation.x);
    vec = vec.rotateY(rotation.y);
    vec = vec.rotateZ(rotation.z);
    
    // Translate
    vec += translation;
    
    return Math::Point3D(vec.x, vec.y, vec.z);
}

Math::Vector3D Transform::applyRotation(const Math::Vector3D &vec) const
{
    Math::Vector3D result = vec;
    result = result.rotateX(rotation.x);
    result = result.rotateY(rotation.y);
    result = result.rotateZ(rotation.z);
    return result;
}

Math::Point3D Transform::inverse(const Math::Point3D &point) const
{
    Math::Vector3D vec(point.x, point.y, point.z);
    
    // Inverse translate
    vec -= translation;
    
    // Inverse rotate (apply negative angles in reverse order)
    vec = vec.rotateZ(-rotation.z);
    vec = vec.rotateY(-rotation.y);
    vec = vec.rotateX(-rotation.x);
    
    // Inverse scale
    vec = Math::Vector3D(
        scale.x != 0 ? vec.x / scale.x : vec.x,
        scale.y != 0 ? vec.y / scale.y : vec.y,
        scale.z != 0 ? vec.z / scale.z : vec.z
    );
    
    return Math::Point3D(vec.x, vec.y, vec.z);
}

Math::Vector3D Transform::inverseRotation(const Math::Vector3D &vec) const
{
    Math::Vector3D result = vec;
    result = result.rotateZ(-rotation.z);
    result = result.rotateY(-rotation.y);
    result = result.rotateX(-rotation.x);
    return result;
}

Math::Vector3D Transform::inverseScale(const Math::Vector3D &vec) const
{
    return Math::Vector3D(
        scale.x != 0 ? vec.x / scale.x : vec.x,
        scale.y != 0 ? vec.y / scale.y : vec.y,
        scale.z != 0 ? vec.z / scale.z : vec.z
    );
}

TransformedPrimitive::TransformedPrimitive
(std::shared_ptr<IPrimitive> prim, const Transform &t)
    : primitive(prim), transform(t)
{
}

HitRecord TransformedPrimitive::hit(const Ray &ray) const
{
    // Transform ray to primitive's local space
    Math::Point3D transformedOrigin = transform.inverse(ray.origin);
    Math::Vector3D transformedDir = transform.inverseRotation(ray.direction);
    transformedDir = transform.inverseScale(transformedDir);
    
    Ray transformedRay(transformedOrigin, transformedDir);
    HitRecord record = primitive->hit(transformedRay);

    // Transform hit point and normal back to world space
    if (record.hit) {
        record.point = transform.apply(record.point);
        record.normal = transform.applyRotation(record.normal);
        record.normal = record.normal * (1.0 / record.normal.length());
    }

    return record;
}
