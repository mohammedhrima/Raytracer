#include "ray.cpp"
#include "interval.cpp"

#ifndef HITTABLE
#define HITTABLE

class Hit_record
{
public:
    Point point;
    Vector normal;
    double x;
    bool front_face;

    void set_face_normal(const Ray &ray, const Vector &outward_normal);
};

class Hittable
{
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray &ray, Interval &ray_t, Hit_record &rec) const = 0;
};

void Hit_record::set_face_normal(const Ray &ray, const Vector &outward_normal)
{
    // check if ray comming from inside or outside
    front_face = dot(ray.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

#endif