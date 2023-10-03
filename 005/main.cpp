#include "camera.cpp"

Color ray_color(const Ray &ray, const Hittable &world)
{
    Hit_record rec;
    Interval val(0, infinity);
    if (world.hit(ray, val, rec))
        return 0.5 * (rec.normal + Color(1, 1, 1));

    Vector unit_direction = unit_vector(ray.direction());
    double a = 0.5 * (unit_direction.get_y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main(void)
{

    // World
    Hittable_list world;
    world.add(make_shared<Sphere>(Point(0, 0, -1), 0.5));
    world.add(make_shared<Sphere>(Point(0, -100.5, -1), 100));

    Camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.width = 400;

    cam.render(world);
}