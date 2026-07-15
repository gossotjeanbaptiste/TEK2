#pragma once

#include "scene.hpp"
#include "camera.hpp"
#include <vector>

namespace RayTracer {
    std::vector<Math::Vector3D> renderMultithreaded(const Scene &scene, const Camera &camera, int width, int height);
}