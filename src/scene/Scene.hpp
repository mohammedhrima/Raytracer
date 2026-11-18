#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "scene/Shape.hpp"

class Scene
{
public:
    std::vector<Shape *> shapes;
    Vec3 lightPosition;
    Color lightColor;

    Scene();
    ~Scene();

    void add(Shape *shape) { shapes.push_back(shape); }
    void addDefaultObjects();

    bool closestHit(const Ray &ray, float tMin, float tMax, Hit &hit) const;
    bool occluded(const Ray &ray, float tMin, float maxDistance) const;

private:
    Scene(const Scene &);
    Scene &operator=(const Scene &);
};

#endif
