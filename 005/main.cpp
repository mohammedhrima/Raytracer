#include "coor.cpp"

class Rec
{
public:
    Coor point;
    Coor normal;
    double t;
    bool front_face;

    void set_face_normal(Ray &ray, Coor &norm)
    {
        front_face = dot(ray.dir, norm) < 0;
        normal = front_face ? norm : -norm;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(Ray &ray, double ray_tmin, double ray_tmax, Rec &rec) const = 0;
};

class Sphere : public hittable
{
public:
    Coor center;
    double radius;
    Sphere(Coor center_, double radius_)
    {
        center = center_;
        radius = radius_;
    };
    bool hit(Ray &ray, double tmin, double tmax, Rec &rec)
    {
        Coor oc = ray.org - center;
        double a = ray.dir.length_squared();
        double half_b = dot(oc, ray.dir);
        double c = oc.length_squared() - radius * radius;
        double delta = half_b * half_b - a * c;

        if (delta < 0.0)
            return false;

        double t = (-half_b - sqrt(delta)) / a;
        if (t <= tmin || t >= tmax)
            t = (-half_b + sqrt(delta)) / a;
        if (t <= tmin || t >= tmax)
            return false;

        rec.t = t;
        rec.point = ray.at(t);
        Coor norm = (rec.point - center) / radius;
        rec.set_face_normal(ray, norm);

        return true;
    };
};

double hit_sphere(Coor center, double R, Ray ray)
{
    Coor oc = ray.org - center;
    double a = dot(ray.dir, ray.dir);
    double b = 2.0 * dot(oc, ray.dir);
    double c = dot(oc, oc) - R * R;
    double delta = b * b - 4 * a * c;
    if (delta < 0.0)
        return -1.0;
    return ((-b - sqrt(delta)) / (2.0 * a));
}

Color ray_color(Ray &ray)
{
    Coor center = Coor(0, 0, -1);
    double R = 0.5;

    double t = hit_sphere(center, R, ray);
    if (t > 0.0)
    {
        Coor n = unit_vector(ray.at(t) - center);
        return 0.5 * Color(n.x + 1, n.y + 1, n.z + 1);
    }
    Coor unit = unit_vector(ray.dir);
    double a = 0.5 * (unit.y + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main()
{
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 800;
    int image_height = (image_width / aspect_ratio);
    if (image_height < 1)
        image_height = 1;

    // Camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * ((double)image_width / image_height);
    Coor camera = Coor(0, 0, 0);

    // viewport vectors
    Coor viewport_u = Coor(viewport_width, 0, 0);
    Coor viewport_v = Coor(0, -viewport_height, 0);

    // pixel to pixel step
    Coor pixel_u = viewport_u / image_width;
    Coor pixel_v = viewport_v / image_height;

    // upper left pixel
    Coor viewport_upper_left = camera - Coor(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    Coor first_pixel = viewport_upper_left + 0.5 * (pixel_u + pixel_v);

    // Render
    std::cout << "P3\n";
    std::cout << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j)
    {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            Coor pixel_center = first_pixel + (i * pixel_u) + (j * pixel_v);
            Coor dir = pixel_center - camera;
            Ray ray(camera, dir);

            Color pixel = ray_color(ray);
            write_color(pixel);
        }
    }
    std::clog << "\rDone.                 \n";
}