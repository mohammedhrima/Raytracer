#include "render/Renderer.hpp"

#include <cfloat>
#include <unistd.h>

namespace
{
    const int MAX_BOUNCES = 5;
    const float HIT_EPSILON = 1e-3f; // avoids self-intersection ("shadow acne")
    const int MAX_THREADS = 64;

    int threadCount()
    {
        long n = sysconf(_SC_NPROCESSORS_ONLN);
        if (n < 1)
            return 1;
        return n > MAX_THREADS ? MAX_THREADS : (int)n;
    }

    Color sky(const Vec3 &direction)
    {
        float a = 0.5f * (direction.y + 1.0f);
        return (1.0f - a) * Color(0.85f, 0.65f, 0.5f) + a * Color(0.25f, 0.35f, 0.65f);
    }
    float reflectance(float cosine, float eta)
    {
        float r0 = (1 - eta) / (1 + eta);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow(1 - cosine, 5.0f);
    }

    uint32_t toPixel(const Color &c)
    {
        uint32_t r = (uint32_t)(255.99f * std::sqrt(std::min(c.x, 1.0f)));
        uint32_t g = (uint32_t)(255.99f * std::sqrt(std::min(c.y, 1.0f)));
        uint32_t b = (uint32_t)(255.99f * std::sqrt(std::min(c.z, 1.0f)));
        return (r << 24) | (g << 16) | (b << 8) | 0xFFu;
    }
}

Renderer::Renderer(const Scene &s, const Camera &c, int w, int h)
    : scene(s), camera(c), width(w), height(h), sampleCount(0),
      accumulated(w * h), pixelBuffer(w * h, 0),
      workers(threadCount()), frameId(0), busyWorkers(0), nextRow(0), stopping(false)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&frameStart, NULL);
    pthread_cond_init(&frameDone, NULL);

    // `workers` is fully sized before any thread starts, so &workers[i] stays valid.
    for (size_t i = 0; i < workers.size(); i++)
    {
        workers[i].renderer = this;
        workers[i].index = (int)i;
        pthread_create(&workers[i].thread, NULL, workerMain, &workers[i]);
    }
}

Renderer::~Renderer()
{
    pthread_mutex_lock(&mutex);
    stopping = true;
    pthread_cond_broadcast(&frameStart);
    pthread_mutex_unlock(&mutex);

    for (size_t i = 0; i < workers.size(); i++)
        pthread_join(workers[i].thread, NULL);

    pthread_cond_destroy(&frameDone);
    pthread_cond_destroy(&frameStart);
    pthread_mutex_destroy(&mutex);
}

void Renderer::reset()
{
    sampleCount = 0;
    std::fill(accumulated.begin(), accumulated.end(), Color());
}

void Renderer::renderFrame()
{
    pthread_mutex_lock(&mutex);
    sampleCount++;
    nextRow = 0;
    busyWorkers = (int)workers.size();
    frameId++;
    pthread_cond_broadcast(&frameStart);
    while (busyWorkers > 0)
        pthread_cond_wait(&frameDone, &mutex);
    pthread_mutex_unlock(&mutex);
}

void *Renderer::workerMain(void *arg)
{
    Worker *worker = static_cast<Worker *>(arg);
    worker->renderer->workerLoop(worker->index);
    return NULL;
}

void Renderer::workerLoop(int index)
{
    Rng rng(index * 9781u + 6271u);
    int seenFrame = 0;

    while (true)
    {
        pthread_mutex_lock(&mutex);
        while (!stopping && frameId == seenFrame)
            pthread_cond_wait(&frameStart, &mutex);
        if (stopping)
        {
            pthread_mutex_unlock(&mutex);
            return;
        }
        seenFrame = frameId;
        pthread_mutex_unlock(&mutex);

        while (true)
        {
            int y = __sync_fetch_and_add(&nextRow, 1);
            if (y >= height)
                break;
            renderRow(y, rng);
        }

        pthread_mutex_lock(&mutex);
        if (--busyWorkers == 0)
            pthread_cond_signal(&frameDone);
        pthread_mutex_unlock(&mutex);
    }
}

void Renderer::renderRow(int y, Rng &rng)
{
    float inverseSamples = 1.0f / (float)sampleCount;
    for (int x = 0; x < width; x++)
    {
        int i = y * width + x;
        accumulated[i] += trace(camera.rayFor(x, y, rng), rng);
        pixelBuffer[i] = toPixel(accumulated[i] * inverseSamples);
    }
}

// `throughput` is how much of the light found later still reaches the camera.
Color Renderer::trace(Ray ray, Rng &rng) const
{
    Color result(0, 0, 0);
    Color throughput(1, 1, 1);

    for (int bounce = 0; bounce < MAX_BOUNCES; bounce++)
    {
        Hit hit;
        if (!scene.closestHit(ray, HIT_EPSILON, FLT_MAX, hit))
        {
            result += throughput * sky(ray.direction);
            break;
        }

        const Material &material = *hit.material;
        Vec3 newDirection;

        if (material.kind == Material::DIFFUSE)
        {
            result += throughput * material.color * directLight(hit);
            newDirection = hit.normal + rng.unitVector();
            if (lengthSquared(newDirection) < 1e-8f)
                newDirection = hit.normal;
        }
        else if (material.kind == Material::MIRROR)
        {
            newDirection = reflect(ray.direction, hit.normal);
        }
        else
        {
            float eta = hit.frontFace ? 1.0f / material.ior : material.ior;
            float cosTheta = std::min(dot(-ray.direction, hit.normal), 1.0f);
            float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
            bool totalInternalReflection = eta * sinTheta > 1.0f;
            if (totalInternalReflection || reflectance(cosTheta, eta) > rng.uniform())
                newDirection = reflect(ray.direction, hit.normal);
            else
                newDirection = refract(ray.direction, hit.normal, eta);
        }

        throughput = throughput * material.color;
        ray = Ray(hit.point, normalize(newDirection));
    }
    return result;
}

Color Renderer::directLight(const Hit &hit) const
{
    Vec3 toLight = scene.lightPosition - hit.point;
    float distance = length(toLight);
    Vec3 lightDirection = toLight / distance;

    float cosine = dot(hit.normal, lightDirection);
    if (cosine <= 0)
        return Color(0, 0, 0);
    if (scene.occluded(Ray(hit.point, lightDirection), HIT_EPSILON, distance))
        return Color(0, 0, 0);
    return cosine * scene.lightColor;
}
