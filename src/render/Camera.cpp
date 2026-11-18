#include "render/Camera.hpp"

namespace
{
    const float PI = 3.14159265358979f;
    const float FOCAL_LENGTH = 1.0f;
}

Camera::Camera(int w, int h) : width(w), height(h), fovDegrees(45.0f)
{
    reset();
}

void Camera::reset()
{
    position = Vec3(0, 1.35f, 4.7f);
    yaw = 0;
    pitch = 0;
    update();
}

void Camera::update()
{
    Vec3 forward = rotateY(rotateX(Vec3(0, -0.2f, -1), pitch), yaw);
    Vec3 worldUp = rotateY(rotateX(Vec3(0, 1, 0), pitch), yaw);

    back = normalize(-forward);
    right = normalize(cross(worldUp, back));
    up = cross(back, right);

    float viewHeight = 2.0f * std::tan(fovDegrees * 0.5f * PI / 180.0f) * FOCAL_LENGTH;
    float viewWidth = viewHeight * (float)width / (float)height;

    Vec3 viewRight = viewWidth * right;
    Vec3 viewDown = -viewHeight * up;
    pixelRight = viewRight / (float)width;
    pixelDown = viewDown / (float)height;

    Vec3 viewCenter = position - FOCAL_LENGTH * back;
    Vec3 topLeft = viewCenter - 0.5f * (viewRight + viewDown);
    firstPixel = topLeft + 0.5f * (pixelRight + pixelDown);
}

void Camera::rotate(float dYaw, float dPitch)
{
    yaw += dYaw;
    pitch += dPitch;
    update();
}

void Camera::move(const Vec3 &local)
{
    position += local.x * right + local.y * up + local.z * back;
    update();
}

Ray Camera::rayFor(int x, int y, Rng &rng) const
{
    // firstPixel is a pixel center, so jitter by +/- half a pixel.
    float px = (float)x + rng.range(-0.5f, 0.5f);
    float py = (float)y + rng.range(-0.5f, 0.5f);
    Vec3 target = firstPixel + px * pixelRight + py * pixelDown;
    return Ray(position, normalize(target - position));
}
