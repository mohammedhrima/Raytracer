#include "scene/Shape.hpp"

namespace
{
    const float PARALLEL_EPSILON = 1e-8f;

    bool hitPlane(const Ray &ray, const Vec3 &point, const Vec3 &n, float tMin, float tMax, float &t)
    {
        float denom = dot(n, ray.direction);
        if (std::fabs(denom) < PARALLEL_EPSILON)
            return false;
        t = dot(point - ray.origin, n) / denom;
        return t > tMin && t < tMax;
    }

    bool hitDisk(const Ray &ray, const Vec3 &center, const Vec3 &n, float radius,
                 float tMin, float tMax, float &t)
    {
        if (!hitPlane(ray, center, n, tMin, tMax, t))
            return false;
        return lengthSquared(ray.at(t) - center) <= radius * radius;
    }
}

void Shape::setHit(Hit &hit, const Ray &ray, float t, const Vec3 &outwardNormal) const
{
    hit.t = t;
    hit.point = ray.at(t);
    hit.frontFace = dot(ray.direction, outwardNormal) < 0;
    hit.normal = hit.frontFace ? outwardNormal : -outwardNormal;
    hit.material = &material;
}

Sphere::Sphere(const Vec3 &c, float r, const Material &m)
    : Shape(m), center(c), radius(r) {}

bool Sphere::intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const
{
    Vec3 oc = ray.origin - center;
    float a = lengthSquared(ray.direction);
    float halfB = dot(oc, ray.direction);
    float c = lengthSquared(oc) - radius * radius;
    float discriminant = halfB * halfB - a * c;
    if (discriminant < 0)
        return false;

    float root = std::sqrt(discriminant);
    float t = (-halfB - root) / a;
    if (t <= tMin || t >= tMax)
    {
        t = (-halfB + root) / a;
        if (t <= tMin || t >= tMax)
            return false;
    }
    setHit(hit, ray, t, (ray.at(t) - center) / radius);
    return true;
}

Plane::Plane(const Vec3 &p, const Vec3 &n, const Material &m)
    : Shape(m), point(p), normal(normalize(n)) {}

bool Plane::intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const
{
    float t;
    if (!hitPlane(ray, point, normal, tMin, tMax, t))
        return false;
    setHit(hit, ray, t, normal);
    return true;
}

Triangle::Triangle(const Vec3 &a_, const Vec3 &b_, const Vec3 &c_, const Material &m)
    : Shape(m), a(a_), b(b_), c(c_), normal(normalize(cross(b_ - a_, c_ - a_))) {}

void Triangle::move(const Vec3 &offset)
{
    a += offset;
    b += offset;
    c += offset;
}

// Moller-Trumbore: solve origin + t*dir = a + u*(b-a) + v*(c-a).
bool Triangle::intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const
{
    Vec3 edge1 = b - a;
    Vec3 edge2 = c - a;
    Vec3 p = cross(ray.direction, edge2);
    float det = dot(edge1, p);
    if (std::fabs(det) < PARALLEL_EPSILON)
        return false;

    float invDet = 1.0f / det;
    Vec3 s = ray.origin - a;
    float u = dot(s, p) * invDet;
    if (u < 0 || u > 1)
        return false;

    Vec3 q = cross(s, edge1);
    float v = dot(ray.direction, q) * invDet;
    if (v < 0 || u + v > 1)
        return false;

    float t = dot(edge2, q) * invDet;
    if (t <= tMin || t >= tMax)
        return false;
    setHit(hit, ray, t, normal);
    return true;
}

Rectangle::Rectangle(const Vec3 &c, const Vec3 &u, const Vec3 &v, const Material &m)
    : Shape(m), corner(c), edgeU(u), edgeV(v), normal(normalize(cross(u, v)))
{
    Vec3 n = cross(u, v);
    w = n / dot(n, n);
}

bool Rectangle::intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const
{
    float t;
    if (!hitPlane(ray, corner, normal, tMin, tMax, t))
        return false;

    Vec3 p = ray.at(t) - corner;
    float alpha = dot(cross(p, edgeV), w);
    float beta = dot(cross(edgeU, p), w);
    if (alpha < 0 || alpha > 1 || beta < 0 || beta > 1)
        return false;
    setHit(hit, ray, t, normal);
    return true;
}

Cylinder::Cylinder(const Vec3 &c, const Vec3 &a, float r, float h, const Material &m)
    : Shape(m), center(c), axis(normalize(a)), radius(r), height(h) {}

// Side: remove the axis component of the ray, then it is a 2D circle test.
// Caps: two disks at center +/- axis * height / 2.
bool Cylinder::intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const
{
    float halfHeight = height * 0.5f;
    float bestT = tMax;
    Vec3 bestNormal;

    Vec3 oc = ray.origin - center;
    float dirAxis = dot(ray.direction, axis);
    float ocAxis = dot(oc, axis);
    Vec3 dirPerp = ray.direction - dirAxis * axis;
    Vec3 ocPerp = oc - ocAxis * axis;

    float a = lengthSquared(dirPerp);
    if (a > PARALLEL_EPSILON)
    {
        float halfB = dot(ocPerp, dirPerp);
        float c = lengthSquared(ocPerp) - radius * radius;
        float discriminant = halfB * halfB - a * c;
        if (discriminant >= 0)
        {
            float root = std::sqrt(discriminant);
            float roots[2] = {(-halfB - root) / a, (-halfB + root) / a};
            for (int i = 0; i < 2; i++)
            {
                float t = roots[i];
                float h = ocAxis + t * dirAxis;
                if (t > tMin && t < bestT && std::fabs(h) <= halfHeight)
                {
                    bestT = t;
                    bestNormal = (ocPerp + t * dirPerp) / radius;
                    break;
                }
            }
        }
    }

    for (int side = -1; side <= 1; side += 2)
    {
        Vec3 capNormal = (float)side * axis;
        Vec3 capCenter = center + halfHeight * capNormal;
        float t;
        if (hitDisk(ray, capCenter, capNormal, radius, tMin, bestT, t))
        {
            bestT = t;
            bestNormal = capNormal;
        }
    }

    if (bestT >= tMax)
        return false;
    setHit(hit, ray, bestT, bestNormal);
    return true;
}

Cone::Cone(const Vec3 &b, const Vec3 &a, float r, float h, const Material &m)
    : Shape(m), base(b), axis(normalize(a)), radius(r), height(h) {}

// A point at height h (0 = base, height = tip) is on the side when
// its distance to the axis equals radius * (height - h) / height.
bool Cone::intersect(const Ray &ray, float tMin, float tMax, Hit &hit) const
{
    float bestT = tMax;
    Vec3 bestNormal;

    Vec3 ob = ray.origin - base;
    float dirAxis = dot(ray.direction, axis);
    float obAxis = dot(ob, axis);
    Vec3 dirPerp = ray.direction - dirAxis * axis;
    Vec3 obPerp = ob - obAxis * axis;

    float k2 = (radius / height) * (radius / height);
    float m = height - obAxis;

    float a = lengthSquared(dirPerp) - k2 * dirAxis * dirAxis;
    float halfB = dot(obPerp, dirPerp) + k2 * m * dirAxis;
    float c = lengthSquared(obPerp) - k2 * m * m;

    if (std::fabs(a) > PARALLEL_EPSILON)
    {
        float discriminant = halfB * halfB - a * c;
        if (discriminant >= 0)
        {
            float root = std::sqrt(discriminant);
            float t0 = (-halfB - root) / a;
            float t1 = (-halfB + root) / a;
            if (t0 > t1)
                std::swap(t0, t1);
            float roots[2] = {t0, t1};
            for (int i = 0; i < 2; i++)
            {
                float t = roots[i];
                float h = obAxis + t * dirAxis;
                if (t > tMin && t < bestT && h >= 0 && h <= height)
                {
                    Vec3 radial = normalize(obPerp + t * dirPerp);
                    bestT = t;
                    bestNormal = normalize(height * radial + radius * axis);
                    break;
                }
            }
        }
    }

    float t;
    if (hitDisk(ray, base, -axis, radius, tMin, bestT, t))
    {
        bestT = t;
        bestNormal = -axis;
    }

    if (bestT >= tMax)
        return false;
    setHit(hit, ray, bestT, bestNormal);
    return true;
}
