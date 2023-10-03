
#ifndef SPHERE
#define SPHERE
#include "hittable_list.cpp"

class Sphere : public Hittable
{
private:
    Point center;
    double radius;

public:
    Sphere(Point _center, double _radius);
    bool hit(const Ray &ray, Interval &ray_t, Hit_record &rec) const;
};

Sphere::Sphere(Point _center, double _radius) : center(_center), radius(_radius) {}

bool Sphere::hit(const Ray &ray, Interval &ray_t, Hit_record &rec) const
{
    Vector OC = ray.origin() - center;
    double a = ray.direction().length_squared();
    double half_b = dot(OC, ray.direction());
    double c = OC.length_squared() - radius * radius;
    double delta = half_b * half_b - a * c;

    if (delta < 0)
        return false;

    // get solution for quadratic formula
    double sqrt_delta = sqrt(delta);
    double x = (-half_b - sqrt_delta) / a;
    if (!ray_t.surrounds(x))
        x = (-half_b + sqrt_delta) / a;
    if (!ray_t.surrounds(x))
        return false;

    rec.x = x;
    rec.point = ray.at(rec.x);
    Vector outward_normal = (rec.point - center) / radius;
    rec.set_face_normal(ray, outward_normal);

    return true;
}
#endif