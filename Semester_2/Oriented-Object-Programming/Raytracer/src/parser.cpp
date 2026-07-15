#include "utility/sceneparser.hpp"
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>
#include <cmath>

using namespace RayTracer;

static bool validateRead(std::istringstream &iss, int lineNum, const std::string &command)
{
    if (iss.fail()) {
        std::cerr << "Error: Invalid format for command '" << command 
                  << "' on line " << lineNum << "\n";
        return false;
    }
    return true;
}

// ============ TRANSFORMATION PARSING ============
bool SceneParser::parseTransform(std::istringstream &iss,
    Transform &pendingTransform, int lineNum)
{
    std::string transformType;
    iss >> transformType;
    if (!validateRead(iss, lineNum, "transform:")) return false;

    if (transformType == "rotate") {
        double x, y, z;
        iss >> x >> y >> z;
        if (!validateRead(iss, lineNum, "transform: rotate")) return false;
        pendingTransform.rotate(Math::Vector3D(x, y, z));
    }
    else if (transformType == "translate") {
        double x, y, z;
        iss >> x >> y >> z;
        if (!validateRead(iss, lineNum, "transform: translate")) return false;
        pendingTransform.translate(Math::Vector3D(x, y, z));
    }
    else if (transformType == "scale") {
        double x, y, z;
        iss >> x >> y >> z;
        if (!validateRead(iss, lineNum, "transform: scale")) return false;
        pendingTransform.setScale(Math::Vector3D(x, y, z));
    }
    else {
        std::cerr << "Error: Unknown transform type '" << transformType 
                  << "' on line " << lineNum << "\n";
        return false;
    }
    return true;
}

// ============ PRIMITIVE PARSING ============
bool SceneParser::parseSphere(std::istringstream &iss,
    Scene &scene, Transform &pendingTransform, int lineNum)
{
    double x, y, z, r, cr, cg, cb, ca = 1.0;
    iss >> x >> y >> z >> r >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "sphere:") || r <= 0.0) {
        std::cerr << "Error: Invalid sphere parameters on line " << lineNum << "\n";
        return false;
    }
    iss >> ca;
    if (iss.fail()) {
        iss.clear();
        ca = 1.0;
    }
    auto sphere = std::make_shared<Sphere>(
        Math::Point3D(x, y, z), r, Math::Vector4D(cr, cg, cb, ca)
    );
    if (pendingTransform.hasTransform())
        scene.addPrimitive(
            std::make_shared<TransformedPrimitive>(sphere, pendingTransform));
    else
        scene.addPrimitive(sphere);
    pendingTransform = Transform();
    return true;
}

bool SceneParser::parsePlane(std::istringstream &iss, Scene &scene, int lineNum)
{
    double x, y, z, nx, ny, nz, cr, cg, cb;
    double ca = 1.0;
    iss >> x >> y >> z >> nx >> ny >> nz >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "plane:")) {
        std::cerr << "Error: Invalid plane parameters on line " << lineNum << "\n";
        return false;
    }
    iss >> ca;
    if (iss.fail()) { iss.clear(); ca = 1.0; }
    scene.addPrimitive(std::make_shared<Plane>(
        Math::Point3D(x, y, z), Math::Vector3D(nx, ny, nz),
        Math::Vector4D(cr, cg, cb, ca)
    ));
    return true;
}

bool SceneParser::parseCylinder(std::istringstream &iss, Scene &scene, int lineNum)
{
    double x, y, z, r, h, ax, ay, az, cr, cg, cb;
    double ca = 1.0;
    iss >> x >> y >> z >> r >> h >> ax >> ay >> az >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "cylinder:") || r <= 0.0) {
        std::cerr << "Error: Invalid cylinder parameters on line " << lineNum << "\n";
        return false;
    }
    (void)h;
    iss >> ca;
    if (iss.fail()) { iss.clear(); ca = 1.0; }
    scene.addPrimitive(std::make_shared<Cylinder>(
        Math::Point3D(x, y, z), r, Math::Vector3D(ax, ay, az),
        Math::Vector4D(cr, cg, cb, ca)
    ));
    return true;
}

bool SceneParser::parseLimitedCylinder(std::istringstream &iss,
    Scene &scene, Transform &pendingTransform, int lineNum)
{
    double x, y, z, r, h, ax, ay, az, cr, cg, cb;
    double ca = 1.0;
    iss >> x >> y >> z >> r >> h >> ax >> ay >> az >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "limited_cylinder:") || r <= 0.0 || h <= 0.0) {
        std::cerr << "Error: Invalid limited_cylinder parameters on line " << lineNum << "\n";
        return false;
    }
    auto cylinder = std::make_shared<LimitedCylinder>(
        Math::Point3D(x, y, z), r, h, Math::Vector3D(ax, ay, az),
        Math::Vector4D(cr, cg, cb, ca)
    );
    if (pendingTransform.hasTransform())
        scene.addPrimitive(
            std::make_shared<TransformedPrimitive>(cylinder, pendingTransform));
    else
        scene.addPrimitive(cylinder);
    pendingTransform = Transform();
    return true;
}

bool SceneParser::parseCone(std::istringstream &iss,
    Scene &scene, Transform &pendingTransform, int lineNum)
{
    double x, y, z, h, br, ax, ay, az, cr, cg, cb;
    double ca = 1.0;
    iss >> x >> y >> z >> h >> br >> ax >> ay >> az >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "cone:") || h <= 0.0 || br <= 0.0) {
        std::cerr << "Error: Invalid cone parameters on line " << lineNum << "\n";
        return false;
    }
    auto cone = std::make_shared<Cone>(
        Math::Point3D(x, y, z), h, br, Math::Vector3D(ax, ay, az),
        Math::Vector4D(cr, cg, cb, ca)
    );
    if (pendingTransform.hasTransform())
        scene.addPrimitive(
            std::make_shared<TransformedPrimitive>(cone, pendingTransform));
    else
        scene.addPrimitive(cone);
    pendingTransform = Transform();
    return true;
}

bool SceneParser::parseLimitedCone(std::istringstream &iss,
    Scene &scene, Transform &pendingTransform, int lineNum)
{
    double x, y, z, h, br, ax, ay, az, cr, cg, cb;
    double ca = 1.0;
    iss >> x >> y >> z >> h >> br >> ax >> ay >> az >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "limited_cone:") || h <= 0.0 || br <= 0.0) {
        std::cerr << "Error: Invalid limited_cone parameters on line " << lineNum << "\n";
        return false;
    }
    auto limCone = std::make_shared<LimitedCone>(
        Math::Point3D(x, y, z), h, br, Math::Vector3D(ax, ay, az),
        Math::Vector4D(cr, cg, cb, ca)
    );
    if (pendingTransform.hasTransform())
        scene.addPrimitive(
            std::make_shared<TransformedPrimitive>(limCone, pendingTransform));
    else
        scene.addPrimitive(limCone);
    pendingTransform = Transform();
    return true;
}

bool SceneParser::parseDisk(std::istringstream &iss, Scene &scene, int lineNum)
{
    double x, y, z, r, nx, ny, nz, cr, cg, cb;
    double ca = 1.0;
    iss >> x >> y >> z >> r >> nx >> ny >> nz >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "disk:") || r <= 0.0) {
        std::cerr << "Error: Invalid disk parameters on line " << lineNum << "\n";
        return false;
    }
    iss >> ca;
    if (iss.fail()) { iss.clear(); ca = 1.0; }
    scene.addPrimitive(std::make_shared<Disk>(
        Math::Point3D(x, y, z), r, Math::Vector3D(nx, ny, nz),
        Math::Vector4D(cr, cg, cb, ca)
    ));
    return true;
}

bool SceneParser::parseTriangle(std::istringstream &iss, Scene &scene, int lineNum)
{
    double p0x, p0y, p0z, p1x, p1y, p1z, p2x, p2y, p2z, cr, cg, cb;
    double ca = 1.0;
    iss >> p0x >> p0y >> p0z >> p1x >> p1y >> p1z >> p2x >> p2y
    >> p2z >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "triangle:")) {
        std::cerr << "Error: Invalid triangle parameters on line " << lineNum << "\n";
        return false;
    }
    iss >> ca;
    if (iss.fail()) { iss.clear(); ca = 1.0; }
    scene.addPrimitive(std::make_shared<Triangle>(
        Math::Point3D(p0x, p0y, p0z), 
        Math::Point3D(p1x, p1y, p1z),
        Math::Point3D(p2x, p2y, p2z),
        Math::Vector4D(cr, cg, cb, ca)
    ));
    return true;
}

bool SceneParser::parseTorus(std::istringstream &iss,
    Scene &scene, Transform &pendingTransform, int lineNum)
{
    double x, y, z, majorR, minorR, cr, cg, cb, ca = 1.0;
    iss >> x >> y >> z >> majorR >> minorR >> cr >> cg >> cb;
    if (!validateRead(iss, lineNum, "torus:") || majorR <= 0.0 || minorR <= 0.0) {
        std::cerr << "Error: Invalid torus parameters on line " << lineNum << "\n";
        return false;
    }
    iss >> ca;
    if (iss.fail()) {
        iss.clear();
        ca = 1.0;
    }
    auto torus = std::make_shared<Torus>(
        Math::Point3D(x, y, z), majorR, minorR, Math::Vector4D(cr, cg, cb, ca)
    );
    if (pendingTransform.hasTransform())
        scene.addPrimitive(
            std::make_shared<TransformedPrimitive>(torus, pendingTransform));
    else
        scene.addPrimitive(torus);
    pendingTransform = Transform();
    return true;
}

// ============ LIGHT PARSING ============
bool SceneParser::parseAmbientLight(std::istringstream &iss, Scene &scene, int lineNum)
{
    double intensity, r, g, b;
    iss >> intensity >> r >> g >> b;
    if (!validateRead(iss, lineNum, "ambient:") || intensity < 0.0) {
        std::cerr << "Error: Invalid ambient light parameters on line " << lineNum << "\n";
        return false;
    }
    scene.addLight(std::make_shared<AmbientLight>(
        intensity, Math::Vector3D(r, g, b)
    ));
    return true;
}

bool SceneParser::parseDirectionalLight(std::istringstream &iss, Scene &scene, int lineNum)
{
    double dx, dy, dz, intensity, r, g, b;
    iss >> dx >> dy >> dz >> intensity >> r >> g >> b;
    if (!validateRead(iss, lineNum, "directional:") || intensity < 0.0) {
        std::cerr << "Error: Invalid directional light parameters on line " << lineNum << "\n";
        return false;
    }
    scene.addLight(std::make_shared<DirectionalLight>(
        Math::Vector3D(dx, dy, dz), Math::Vector3D(r, g, b), intensity
    ));
    return true;
}

bool SceneParser::parseAmbientOcclusion(std::istringstream &iss, Scene &scene, int lineNum)
{
    double intensity, r, g, b;
    int samples;
    double maxDistance;
    
    iss >> intensity >> r >> g >> b >> samples >> maxDistance;
    if (!validateRead(iss, lineNum, "ambientocclusion:") ||
    intensity < 0.0 || samples <= 0 || maxDistance <= 0.0) {
        std::cerr << "Error: Invalid ambient occlusion parameters on line " << lineNum << "\n";
        return false;
    }
    auto ao = std::make_shared<AmbientOcclusion>(
        intensity, Math::Vector3D(r, g, b), samples, maxDistance
    );
    scene.addLight(ao);
    return true;
}

// ============ CAMERA PARSING ============
bool SceneParser::parseCamera(std::istringstream &iss,
    Camera &camera, int &width, int &height, int lineNum)
{
    double px, py, pz, fov;
    iss >> px >> py >> pz >> fov >> width >> height;
    if (!validateRead(iss, lineNum, "camera:") || fov <= 0.0 || width <= 0 || height <= 0) {
        std::cerr << "Error: Invalid camera parameters on line " << lineNum << "\n";
        return false;
    }
    
    Math::Point3D pos(px, py, pz);
    double aspectRatio = static_cast<double>(width) / static_cast<double>(height);
    double fovRadians = fov * M_PI / 360.0;
    double viewportHeight = 2.0 * std::tan(fovRadians);
    double viewportWidth = viewportHeight * aspectRatio;
    
    double focalDistance = 1.0;
    Math::Point3D viewportOrigin(
        pos.x - viewportWidth / 2.0,
        pos.y - viewportHeight / 2.0,
        pos.z + focalDistance
    );
    
    camera = Camera(
        Rectangle3D(
            viewportOrigin,
            Math::Vector3D(viewportWidth, 0, 0),
            Math::Vector3D(0, viewportHeight, 0)
        )
    );
    camera.origin = pos;
    return true;
}

// ============ MAIN SCENE LOADER ============
bool SceneParser::loadScene(const std::string &filename,
    Scene &scene, Camera &camera, int &width, int &height)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open scene file: " << filename << "\n";
        return false;
    }
    width = 1920;
    height = 1080;
    bool cameraFound = false;
    Transform pendingTransform;
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        bool parseSuccess = true;
        if (command == "transform:") {
            parseSuccess = parseTransform(iss, pendingTransform, lineNum);
        } else if (command == "sphere:") {
            parseSuccess = parseSphere(iss, scene, pendingTransform, lineNum);
        } else if (command == "plane:") {
            parseSuccess = parsePlane(iss, scene, lineNum);
        } else if (command == "cylinder:") {
            parseSuccess = parseCylinder(iss, scene, lineNum);
        } else if (command == "limited_cylinder:") {
            parseSuccess = parseLimitedCylinder(iss, scene, pendingTransform, lineNum);
        } else if (command == "cone:") {
            parseSuccess = parseCone(iss, scene, pendingTransform, lineNum);
        } else if (command == "limited_cone:") {
            parseSuccess = parseLimitedCone(iss, scene, pendingTransform, lineNum);
        } else if (command == "torus:") {
            parseSuccess = parseTorus(iss, scene, pendingTransform, lineNum);
        } else if (command == "disk:") {
            parseSuccess = parseDisk(iss, scene, lineNum);
        } else if (command == "triangle:") {
            parseSuccess = parseTriangle(iss, scene, lineNum);
        } else if (command == "ambient:") {
            parseSuccess = parseAmbientLight(iss, scene, lineNum);
        } else if (command == "directional:") {
            parseSuccess = parseDirectionalLight(iss, scene, lineNum);
        } else if (command == "ambientocclusion:") {
            parseSuccess = parseAmbientOcclusion(iss, scene, lineNum);
        } else if (command == "camera:") {
            cameraFound = parseCamera(iss, camera, width, height, lineNum);
            parseSuccess = cameraFound;
        } else if (command != "#") {
            std::cerr << "Warning: Unknown command '" << command << "' on line " << lineNum << "\n";
        } else {
            validateRead(iss, lineNum, command);
        }
        if (!parseSuccess) {
            file.close();
            return false;
        }
    }
    if (!cameraFound) {
        std::cerr << "Error: No camera defined in scene file\n";
        file.close();
        return false;
    }
    file.close();
    return true;
}

 // Namespace RayTracer
