#include "utility/multithreading.hpp"
#include <algorithm>
#include <atomic>
#include <thread>

using namespace RayTracer;

static void preparePixelTasks(int width, int height, 
    std::vector<int> &pixelRows, std::vector<int> &pixelColumns)
{
    std::size_t taskIndex = 0;
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            pixelRows[taskIndex] = y;
            pixelColumns[taskIndex] = x;
            ++taskIndex;
        }
    }
}

static void renderPixelTask(const Scene &scene, const Camera &camera, 
    int width, int height, int x, int y, Math::Vector3D &pixelColor)
{
    double u = static_cast<double>(x) / (width - 1);
    double v = static_cast<double>(y) / (height - 1);
    Ray ray = camera.ray(u, v);
    pixelColor = scene.traceRayWithTransparency(ray, 10);  // Max recursion depth of 10
}

static void workerThread(const Scene &scene, const Camera &camera,
    int width, int height, std::atomic<std::size_t> &nextTask,
    std::size_t totalTasks, const std::vector<int> &pixelRows,
    const std::vector<int> &pixelColumns, std::vector<Math::Vector3D> &renderedPixels)
{
    while (true) {
        std::size_t currentTask = nextTask.fetch_add(1, std::memory_order_relaxed);
        if (currentTask >= totalTasks)
            break;
        
        int x = pixelColumns[currentTask];
        int y = pixelRows[currentTask];
        renderPixelTask(scene, camera, width, height, x, y, renderedPixels[currentTask]);
    }
}

std::vector<Math::Vector3D> RayTracer::renderMultithreaded(const Scene &scene, const Camera &camera, int width, int height)
{
    const std::size_t totalTasks = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    std::vector<Math::Vector3D> renderedPixels(totalTasks);
    std::vector<int> pixelRows(totalTasks);
    std::vector<int> pixelColumns(totalTasks);

    preparePixelTasks(width, height, pixelRows, pixelColumns);

    std::atomic<std::size_t> nextTask(0);
    const unsigned int workerCount = std::max(1u, std::min(std::thread::hardware_concurrency() - 1, static_cast<unsigned int>(totalTasks)));
    std::vector<std::thread> workers;
    workers.reserve(workerCount);

    for (unsigned int i = 0; i < workerCount; ++i) {
        workers.emplace_back(workerThread, std::ref(scene), std::ref(camera),
            width, height, std::ref(nextTask), totalTasks,
            std::ref(pixelRows), std::ref(pixelColumns), std::ref(renderedPixels));
    }

    for (std::thread &worker : workers)
        worker.join();

    return renderedPixels;
}