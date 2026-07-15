#include "../../include/lights/ambientocclusion.hpp"
#include "../../include/utility/scene.hpp"
#include "../../include/utility/ray.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace RayTracer;

AmbientOcclusion::AmbientOcclusion(double intens, const Math::Vector3D &col, int numSamples, double maxDist)
    : intensity(intens), color(col), samples(numSamples), maxDistance(maxDist), scene(nullptr)
{
    srand(time(nullptr)); // Seed for random numbers
}

void AmbientOcclusion::setScene(const Scene* scenePtr)
{
    scene = scenePtr;
}

Math::Vector3D AmbientOcclusion::getIntensity(const Math::Point3D &point) const
{
    (void)point;
    return Math::Vector3D(
        color.x * intensity,
        color.y * intensity,
        color.z * intensity
    );
}

Math::Vector3D AmbientOcclusion::getDirection(const Math::Point3D &point) const
{
    (void)point;
    return Math::Vector3D(0, 0, 0);
}

double AmbientOcclusion::calculateOcclusion(const Math::Point3D &point, const Math::Vector3D &normal) const
{
    if (scene == nullptr || samples <= 0)
        return 1.0; // No occlusion if scene not set
    
    int occludedSamples = 0;
    
    for (int i = 0; i < samples; ++i) {
        // Generate random direction in hemisphere
        Math::Vector3D direction = randomHemisphereDirection(normal);
        
        // Create ray from the point in that direction with small offset to avoid self-intersection
        Ray testRay(point + normal * 0.001, direction);
        
        // Test if ray hits anything before maxDistance
        HitRecord hit = scene->raycast(testRay);
        
        if (hit.hit && hit.t > 0.0 && hit.t < maxDistance) {
            occludedSamples++;
        }
    }
    
    // Return occlusion factor (0 = fully occluded, 1 = fully lit)
    return 1.0 - (static_cast<double>(occludedSamples) / samples);
}

Math::Vector3D AmbientOcclusion::randomHemisphereDirection(const Math::Vector3D &normal) const
{
    // Generate random point in unit sphere until we get one in hemisphere
    Math::Vector3D randomDir;
    do {
        randomDir = Math::Vector3D(
            2.0 * rand() / RAND_MAX - 1.0,
            2.0 * rand() / RAND_MAX - 1.0,
            2.0 * rand() / RAND_MAX - 1.0
        );
    } while (randomDir.dot(randomDir) > 1.0); // Reject if outside unit sphere
    
    // Normalize to get random direction
    randomDir = randomDir / randomDir.length();
    
    // If pointing opposite to normal, flip it
    if (randomDir.dot(normal) < 0.0) {
        randomDir = randomDir * -1.0;
    }
    
    return randomDir;
}
