#include "utility/scene.hpp"
#include <algorithm>

using namespace RayTracer;

void Scene::addPrimitive(std::shared_ptr<IPrimitive> prim)
{
    primitives.push_back(prim);
}

void Scene::addLight(std::shared_ptr<ILight> light)
{
    lights.push_back(light);
}

void Scene::setBackgroundColor(const Math::Vector3D &color)
{
    backgroundColor = color;
}

HitRecord Scene::raycast(const Ray &ray) const
{
    HitRecord closest;
    closest.hit = false;
    closest.t = 1e9;

    for (const auto &prim : primitives) {
        HitRecord record = prim->hit(ray);
        if (record.hit && record.t < closest.t)
            closest = record;
    }

    return closest;
}

void Scene::applyAmbientLighting(const HitRecord &record,
    Math::Vector3D &finalColor) const
{
    for (const auto &light : lights) {
        if (auto ambient = std::dynamic_pointer_cast<AmbientLight>(light)) {
            finalColor += ambient->getIntensity(record.point);
        }
    }
}

void Scene::applyAmbientOcclusion(const HitRecord &record,
    Math::Vector3D &finalColor) const
{
    for (const auto &light : lights) {
        if (auto ao = std::dynamic_pointer_cast<AmbientOcclusion>(light)) {
            const_cast<AmbientOcclusion*>(ao.get())->setScene(this);
            double aoFactor = ao->calculateOcclusion(record.point, record.normal);
            Math::Vector3D aoColor = ao->getIntensity(record.point);
            finalColor += Math::Vector3D(
                aoColor.x * aoFactor,
                aoColor.y * aoFactor,
                aoColor.z * aoFactor
            );
        }
    }
}

void Scene::applyDirectionalLighting
    (const HitRecord &record, Math::Vector3D &finalColor) const
{
    for (const auto &light : lights) {
        if (auto dir = std::dynamic_pointer_cast<DirectionalLight>(light)) {
            Math::Vector3D lightDir = dir->getDirection(record.point) * -1.0;
            double diffuse = std::max(0.0, record.normal.dot(lightDir));
            Math::Vector3D intensity = dir->getIntensity(record.point);
            finalColor += Math::Vector3D(
                intensity.x * diffuse,
                intensity.y * diffuse,
                intensity.z * diffuse
            );
        }
    }
}

void Scene::applyMaterialColor
    (Math::Vector3D &finalColor, const Math::Vector3D &materialColor) const
{
    finalColor = Math::Vector3D(
        std::min(255.0, finalColor.x * materialColor.x / 255.0),
        std::min(255.0, finalColor.y * materialColor.y / 255.0),
        std::min(255.0, finalColor.z * materialColor.z / 255.0)
    );
}

void Scene::applyMaterialColorWithAlpha
    (Math::Vector3D &finalColor, const Math::Vector4D &materialColor) const
{
    // Extraire les RGB du Vector4D et appliquer la couleur du matériau
    Math::Vector3D materialRGB(materialColor.x, materialColor.y, materialColor.z);
    applyMaterialColor(finalColor, materialRGB);
    
    // Puis faire le blending avec alpha: finalColor = (1-alpha)*background + alpha*finalColor
    double alpha = materialColor.w;
    finalColor = Math::Vector3D(
        (1.0 - alpha) * backgroundColor.x + alpha * finalColor.x,
        (1.0 - alpha) * backgroundColor.y + alpha * finalColor.y,
        (1.0 - alpha) * backgroundColor.z + alpha * finalColor.z
    );
}

Math::Vector3D Scene::shade(const HitRecord &record) const
{
    if (!record.hit)
        return backgroundColor;
    Math::Vector3D finalColor(0, 0, 0);
    applyAmbientLighting(record, finalColor);
    applyAmbientOcclusion(record, finalColor);
    applyDirectionalLighting(record, finalColor);
    applyMaterialColorWithAlpha(finalColor, record.color);
    return finalColor;
}

Math::Vector3D Scene::traceRayWithTransparency(const Ray &ray, int maxDepth) const
{
    if (maxDepth <= 0)
        return backgroundColor;
    
    HitRecord record = raycast(ray);
    if (!record.hit)
        return backgroundColor;
    
    // Shade the hit (lighting + material RGB)
    Math::Vector3D hitColor(0, 0, 0);
    applyAmbientLighting(record, hitColor);
    applyAmbientOcclusion(record, hitColor);
    applyDirectionalLighting(record, hitColor);
    
    // Apply material color (multiplicative blend)
    Math::Vector3D materialRGB(record.color.x, record.color.y, record.color.z);
    applyMaterialColor(hitColor, materialRGB);
    
    // Handle transparency by continuing the ray
    double alpha = record.color.w;
    if (alpha < 0.999) {  // If semi-transparent (threshold to avoid floating point issues)
        // Continue ray through the object
        const double epsilon = 1e-4;
        Math::Point3D newOrigin = record.point + ray.direction * epsilon;
        Ray continuedRay(newOrigin, ray.direction);
        
        // Recursively trace what's behind
        Math::Vector3D behindColor = traceRayWithTransparency(continuedRay, maxDepth - 1);
        
        // Blend: finalColor = (1 - alpha) * behind + alpha * hitColor
        hitColor = Math::Vector3D(
            (1.0 - alpha) * behindColor.x + alpha * hitColor.x,
            (1.0 - alpha) * behindColor.y + alpha * hitColor.y,
            (1.0 - alpha) * behindColor.z + alpha * hitColor.z
        );
    }
    
    return hitColor;
}

