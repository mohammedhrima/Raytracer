#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <pthread.h>
#include <stdint.h>
#include <vector>

#include "render/Camera.hpp"
#include "scene/Scene.hpp"

// Progressive path tracer: each renderFrame() adds one sample per pixel
// and displays the running average.
class Renderer
{
public:
    Renderer(const Scene &scene, const Camera &camera, int width, int height);
    ~Renderer();

    void renderFrame();
    void reset();
    int samples() const { return sampleCount; }
    const uint32_t *pixels() const { return &pixelBuffer[0]; }

private:
    struct Worker
    {
        Renderer *renderer;
        int index;
        pthread_t thread;
    };

    const Scene &scene;
    const Camera &camera;
    int width;
    int height;
    int sampleCount;
    std::vector<Color> accumulated;
    std::vector<uint32_t> pixelBuffer;

    std::vector<Worker> workers;
    pthread_mutex_t mutex;
    pthread_cond_t frameStart;
    pthread_cond_t frameDone;
    int frameId;
    int busyWorkers;
    int nextRow;
    bool stopping;

    static void *workerMain(void *arg);
    void workerLoop(int index);
    void renderRow(int y, Rng &rng);
    Color trace(Ray ray, Rng &rng) const;
    Color directLight(const Hit &hit) const;

    Renderer(const Renderer &);
    Renderer &operator=(const Renderer &);
};

#endif
