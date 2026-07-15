#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include "raytracer.hpp"

const int MAX_COLOR = 255;

void printUsage(const char *prog) {
    std::cerr << "USAGE: " << prog << " <SCENE_FILE>\n";
    std::cerr << "SCENE_FILE: scene configuration\n";
}

bool checkArguments(int argc, char **argv) {
    bool isHelp = (argc == 2 && (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0));
    bool isError = (argc != 2);

    if (isHelp) {
        std::cout << "USAGE: ./raytracer <SCENE_FILE>\n";
        std::cout << "SCENE_FILE: scene configuration\n";
        return false;
    }
    if (isError) {
        printUsage(argv[0]);
        return false;
    }
    return true;
}

std::string getOutputFilename() {
    std::string output_filename = "output.ppm";
    std::cout << "Output filename [output.ppm]: ";
    std::cout.flush();
    
    std::string user_input;
    if (std::getline(std::cin, user_input) && !user_input.empty()) {
        output_filename = user_input;
    }
    return output_filename;
}

bool loadScene(const char *filename, RayTracer::Scene &scene, RayTracer::Camera &cam, int &width, int &height) {
    if (!RayTracer::SceneParser::loadScene(filename, scene, cam, width, height)) {
        std::cerr << "Error: Failed to load scene from " << filename << "\n";
        return false;
    }
    return true;
}

void writePPMHeader(std::ofstream &file, int width, int height) {
    file << "P3\n";
    file << width << " " << height << "\n";
    file << MAX_COLOR << "\n";
}

void writePPMPixels(std::ofstream &file, const std::vector<Math::Vector3D> &pixels) {
    for (const Math::Vector3D &color : pixels) {
        int r = static_cast<int>(std::max(0.0, std::min(255.0, color.x)));
        int g = static_cast<int>(std::max(0.0, std::min(255.0, color.y)));
        int b = static_cast<int>(std::max(0.0, std::min(255.0, color.z)));
        file << r << " " << g << " " << b << "\n";
    }
}

int main(int argc, char **argv)
{
    if (!checkArguments(argc, argv))
        return 84;

    int width = 1920;
    int height = 1080;

    std::string output_filename = getOutputFilename();
    std::ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not open " << output_filename << " for writing\n";
        return 84;
    }

    RayTracer::Scene scene;
    RayTracer::Camera cam(
        RayTracer::Rectangle3D(
            Math::Point3D(-1.0, -1.0, -1.0),
            Math::Vector3D(2.0, 0.0, 0.0),
            Math::Vector3D(0.0, 2.0, 0.0)
        )
    );

    if (!loadScene(argv[1], scene, cam, width, height))
        return 84;

    writePPMHeader(output_file, width, height);
    std::vector<Math::Vector3D> renderedPixels = RayTracer::renderMultithreaded(scene, cam, width, height);
    writePPMPixels(output_file, renderedPixels);

    output_file.close();
    std::cout << "Image generated: " << output_filename << " (" << width << "x" << height << ")\n";
    return 0;
}