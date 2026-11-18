#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "math/Random.hpp"
#include "math/Vec3.hpp"

struct Camera
{
    int width;
    int height;
    float fovDegrees;
    Vec3 position;
    float yaw;
    float pitch;

    Vec3 right;
    Vec3 up;
    Vec3 back;
    Vec3 firstPixel;
    Vec3 pixelRight;
    Vec3 pixelDown;

    Camera(int width, int height);

    void reset();
    void update();
    void rotate(float dYaw, float dPitch);
    void move(const Vec3 &local);

    Ray rayFor(int x, int y, Rng &rng) const;
};

#endif
