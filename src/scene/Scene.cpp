#include "scene/Scene.hpp"

namespace
{
    Color rgb(float r, float g, float b)
    {
        return Color(r / 255.0f, g / 255.0f, b / 255.0f);
    }
}

Scene::Scene() : lightPosition(-4, 6, 4), lightColor(1.0f, 0.9f, 0.75f) {}

Scene::~Scene()
{
    for (size_t i = 0; i < shapes.size(); i++)
        delete shapes[i];
}

bool Scene::closestHit(const Ray &ray, float tMin, float tMax, Hit &hit) const
{
    bool found = false;
    float closest = tMax;
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->intersect(ray, tMin, closest, hit))
        {
            found = true;
            closest = hit.t;
        }
    }
    return found;
}

bool Scene::occluded(const Ray &ray, float tMin, float maxDistance) const
{
    Hit ignored;
    for (size_t i = 0; i < shapes.size(); i++)
    {
        if (shapes[i]->intersect(ray, tMin, maxDistance, ignored))
            return true;
    }
    return false;
}

void Scene::addDefaultObjects()
{
    const Material::Kind DIFFUSE = Material::DIFFUSE;
    const Material::Kind MIRROR = Material::MIRROR;
    const Material::Kind GLASS = Material::GLASS;

    // Keys 0-9 select shapes in insertion order.
    add(new Sphere(Vec3(0, 0.75f, 0), 0.75f, Material(GLASS, Color(1, 1, 1), 1.5f)));
    add(new Sphere(Vec3(-1.45f, 0.6f, -0.7f), 0.6f, Material(MIRROR, Color(0.95f, 0.95f, 0.95f))));
    add(new Cylinder(Vec3(1.35f, 0.35f, -0.4f), Vec3(0, 1, 0), 0.45f, 0.7f, Material(DIFFUSE, rgb(40, 150, 160))));
    add(new Cone(Vec3(1.35f, 0.7f, -0.4f), Vec3(0, 1, 0), 0.4f, 0.9f, Material(DIFFUSE, rgb(245, 150, 60))));

    // An arc of small colored spheres in front.
    const Color palette[] = {rgb(240, 90, 90), rgb(250, 190, 60), rgb(110, 210, 140),
                             rgb(80, 160, 240), rgb(170, 110, 230)};
    for (int i = 0; i < 5; i++)
    {
        float angle = (i - 2) * 0.38f;
        Vec3 center(1.4f * std::sin(angle), 0.2f, 1.3f - 0.9f * (1.0f - std::cos(angle)));
        add(new Sphere(center, 0.2f, Material(DIFFUSE, palette[i])));
    }

    add(new Plane(Vec3(0, 0, 0), Vec3(0, 1, 0), Material(DIFFUSE, rgb(160, 145, 135))));
    add(new Plane(Vec3(0, 0, -4), Vec3(0, 0, 1), Material(DIFFUSE, rgb(30, 40, 75))));
    add(new Triangle(Vec3(-2.4f, 0, -2.6f), Vec3(-0.9f, 0, -2.9f), Vec3(-1.6f, 2.0f, -2.75f),
                     Material(DIFFUSE, rgb(250, 210, 90))));
}
