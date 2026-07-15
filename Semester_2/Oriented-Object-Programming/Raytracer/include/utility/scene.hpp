#pragma once

#include <vector>
#include <memory>
#include "../primitives/iprimitive.hpp"
#include "../lights/light.hpp"

namespace RayTracer {
    class Scene {
        private:
            std::vector<std::shared_ptr<IPrimitive>> primitives;
            std::vector<std::shared_ptr<ILight>> lights;
            Math::Vector3D backgroundColor;

            // Shading helper functions
            void applyAmbientLighting(const HitRecord &record, Math::Vector3D &finalColor) const;
            void applyAmbientOcclusion(const HitRecord &record, Math::Vector3D &finalColor) const;
            void applyDirectionalLighting(const HitRecord &record, Math::Vector3D &finalColor) const;
            void applyMaterialColor(Math::Vector3D &finalColor, const Math::Vector3D &materialColor) const;
            void applyMaterialColorWithAlpha(Math::Vector3D &finalColor, const Math::Vector4D &materialColor) const;

        public:
            Scene() : backgroundColor(0, 0, 0) {}

            void addPrimitive(std::shared_ptr<IPrimitive> prim);

            void addLight(std::shared_ptr<ILight> light);

            void setBackgroundColor(const Math::Vector3D &color);

            HitRecord raycast(const Ray &ray) const;

            Math::Vector3D shade(const HitRecord &record) const;

            Math::Vector3D traceRayWithTransparency(const Ray &ray, int maxDepth = 10) const;
    };
}
