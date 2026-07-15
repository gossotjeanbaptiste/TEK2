#pragma once

#include "ILight.hpp"
#include "../utility/math/math.hpp"
#include <memory>

namespace RayTracer {
    class Scene; // Forward declaration
    
    class AmbientOcclusion : public ILight {
        private:
            double intensity;
            Math::Vector3D color;
            int samples;
            double maxDistance;
            
        public:
            AmbientOcclusion(double intens, const Math::Vector3D &col, int numSamples = 16, double maxDist = 10.0);
            
            Math::Vector3D getIntensity(const Math::Point3D &point) const override;
            Math::Vector3D getDirection(const Math::Point3D &point) const override;
            
            // Set the scene for occlusion testing
            void setScene(const Scene* scenePtr);
            
            // Calculate occlusion factor (0 = fully occluded, 1 = fully lit)
            double calculateOcclusion(const Math::Point3D &point, const Math::Vector3D &normal) const;
            
        private:
            const Scene* scene;
            
            // Generate a random direction in the hemisphere around the normal
            Math::Vector3D randomHemisphereDirection(const Math::Vector3D &normal) const;
    };
}