#include "hittable.cpp"
#include "interval.cpp"

#ifndef HITTABLE_LIST
#define HITTABLE_LIST

using std::make_shared;
using std::shared_ptr;

class Hittable_list : public Hittable
{
public:
    std::vector<shared_ptr<Hittable>> objects;

    Hittable_list();
    Hittable_list(shared_ptr<Hittable> object);
    void clear();
    void add(shared_ptr<Hittable> object);
    bool hit(const Ray &ray, Interval &ray_t, Hit_record &rec) const;
};

Hittable_list::Hittable_list() {}

Hittable_list::Hittable_list(shared_ptr<Hittable> object)
{
    add(object);
}

void Hittable_list::clear()
{
    objects.clear();
}

void Hittable_list::add(shared_ptr<Hittable> object)
{
    objects.push_back(object);
}

bool Hittable_list::hit(const Ray &ray, Interval &ray_t, Hit_record &rec) const
{
    Hit_record tmp_rec;
    bool did_hit = false;
    double closest = ray_t.max;

    for (const auto &object : objects)
    {
        Interval val(ray_t.min, closest);
        if (object->hit(ray, val, tmp_rec))
        {
            did_hit = true;
            closest = tmp_rec.x;
            rec = tmp_rec;
        }
    }
    return did_hit;
}

#endif