#pragma once

#include "scene.hpp"
#include "camera.hpp"
#include "math/math.hpp"
#include "../primitives/primitives.hpp"
#include "../lights/light.hpp"
#include <string>
#include <sstream>

namespace RayTracer {
    class SceneParser {
        public:
            static bool loadScene
            (const std::string &filename, Scene &scene, Camera &camera, int &width, int &height);
            static void createDemoScene(Scene &scene);
        
        private:
            // Transformation parsing
            static bool parseTransform(std::istringstream &iss, Transform &pendingTransform, int lineNum);
            // Primitive parsing
            static bool parseSphere(std::istringstream &iss, Scene &scene, Transform &pendingTransform, int lineNum);
            static bool parsePlane(std::istringstream &iss, Scene &scene, int lineNum);
            static bool parseCylinder(std::istringstream &iss, Scene &scene, int lineNum);
            static bool parseLimitedCylinder(std::istringstream &iss, Scene &scene, Transform &pendingTransform, int lineNum);
            static bool parseCone(std::istringstream &iss, Scene &scene, Transform &pendingTransform, int lineNum);
            static bool parseLimitedCone(std::istringstream &iss, Scene &scene, Transform &pendingTransform, int lineNum);
            static bool parseDisk(std::istringstream &iss, Scene &scene, int lineNum);
            static bool parseTriangle(std::istringstream &iss, Scene &scene, int lineNum);
            static bool parseTorus(std::istringstream &iss, Scene &scene, Transform &pendingTransform, int lineNum);
            
            // Light parsing
            static bool parseAmbientLight(std::istringstream &iss, Scene &scene, int lineNum);
            static bool parseDirectionalLight(std::istringstream &iss, Scene &scene, int lineNum);
            static bool parseAmbientOcclusion(std::istringstream &iss, Scene &scene, int lineNum);
            
            // Camera parsing
            static bool parseCamera(std::istringstream &iss, Camera &camera, int &width, int &height, int lineNum);
    };
}
