#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "math/Vec3.hpp"

struct Material
{
    enum Kind
    {
        DIFFUSE,
        MIRROR,
        GLASS
    };

    Kind kind;
    Color color;
    float ior;

    Material() : kind(DIFFUSE), color(1, 1, 1), ior(1.5f) {}
    Material(Kind k, const Color &c, float i = 1.5f) : kind(k), color(c), ior(i) {}
};

struct Hit
{
    float t;
    Vec3 point;
    Vec3 normal;
    bool frontFace;
    const Material *material;
};

struct Shape
{
    Material material;

    explicit Shape(const Material &m) : material(m) {}
    virtual ~Shape() {}

    // Returns true when the ray hits this shape with tMin < t < tMax,
    // and only then fills `hit`.
    virtual bool intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const = 0;
    virtual void move(const Vec3 &offset) = 0;
    virtual const char *name() const = 0;

protected:
    void setHit(Hit &hit, const Ray &ray, float t, const Vec3 &outwardNormal) const;
};

struct Sphere : Shape
{
    Vec3 center;
    float radius;

    Sphere(const Vec3 &center, float radius, const Material &m);
    bool intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const;
    void move(const Vec3 &offset) { center += offset; }
    const char *name() const { return "sphere"; }
};

struct Plane : Shape
{
    Vec3 point;
    Vec3 normal;

    Plane(const Vec3 &point, const Vec3 &normal, const Material &m);
    bool intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const;
    void move(const Vec3 &offset) { point += offset; }
    const char *name() const { return "plane"; }
};

struct Triangle : Shape
{
    Vec3 a;
    Vec3 b;
    Vec3 c;
    Vec3 normal;

    Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c, const Material &m);
    bool intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const;
    void move(const Vec3 &offset);
    const char *name() const { return "triangle"; }
};

struct Rectangle : Shape
{
    Vec3 corner;
    Vec3 edgeU;
    Vec3 edgeV;
    Vec3 normal;
    Vec3 w; // cross(edgeU, edgeV) / |cross|^2, used to find the hit's (alpha, beta)

    Rectangle(const Vec3 &corner, const Vec3 &edgeU, const Vec3 &edgeV, const Material &m);
    bool intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const;
    void move(const Vec3 &offset) { corner += offset; }
    const char *name() const { return "rectangle"; }
};

struct Cylinder : Shape
{
    Vec3 center;
    Vec3 axis;
    float radius;
    float height;

    Cylinder(const Vec3 &center, const Vec3 &axis, float radius, float height, const Material &m);
    bool intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const;
    void move(const Vec3 &offset) { center += offset; }
    const char *name() const { return "cylinder"; }
};

// Closed cone. `base` is the center of the base disk, the tip is at base + height * axis.
struct Cone : Shape
{
    Vec3 base;
    Vec3 axis;
    float radius;
    float height;

    Cone(const Vec3 &base, const Vec3 &axis, float radius, float height, const Material &m);
    bool intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const;
    void move(const Vec3 &offset) { base += offset; }
    const char *name() const { return "cone"; }
};

#endif
