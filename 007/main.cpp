#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <cstdlib>

using std::sqrt;
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}
// random number in [0,1[
inline double random_double()
{
    return rand() / (RAND_MAX + 1.0);
}
// random number in [min,max[
inline double random_double(double min, double max)
{
    return min + (max - min) * random_double();
};

class Vector
{
public:
    // attributes
    double e[3];
    // constractors
    Vector();
    Vector(double e1, double e2, double e3);
    // methods
    double length() const;
    double length_squared() const;
    static Vector random();
    static Vector random(double min, double max);
    // operators oveloading
    Vector operator-() const;
    double operator[](int i) const;
    double operator[](char c) const;
    Vector &operator+=(const Vector &v);
    Vector &operator*=(double t);
    Vector &operator/=(double t);
};

// constractors
Vector::Vector() : e{0, 0, 0} {};
Vector::Vector(double e1, double e2, double e3) : e{e1, e2, e3} {};
// methods
double Vector::length() const
{
    return sqrt(length_squared());
}
double Vector::length_squared() const
{
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}
Vector Vector::random()
{
    return Vector(random_double(), random_double(), random_double());
}
Vector Vector::random(double min, double max)
{
    return Vector(random_double(min, max), random_double(min, max), random_double(min, max));
}
// operators overloading
Vector Vector::operator-() const
{
    return Vector(-e[0], -e[1], -e[2]);
}
double Vector::operator[](int i) const
{
    return e[i];
}
double Vector::operator[](char c) const
{
    switch (c)
    {
    case 'x':
    case 'r':
        return e[0];
    case 'y':
    case 'g':
        return e[1];
    case 'z':
    case 'b':
        return e[2];
    default:
        return -100.0;
    }
}
Vector &Vector::operator+=(const Vector &v)
{
    for (int i = 0; i < 3; i++)
        e[i] += v.e[i];
    return *this;
}
Vector &Vector::operator*=(double t)
{
    for (int i = 0; i < 3; i++)
        e[i] *= t;
    return *this;
}
Vector &Vector::operator/=(double t)
{
    return *this *= 1 / t;
}
inline std::ostream &operator<<(std::ostream &out, const Vector &v)
{
    return out << v['x'] << ' ' << v['y'] << ' ' << v['z'];
}
inline Vector operator+(const Vector &u, const Vector &v)
{
    return Vector(u['x'] + v['x'], u['y'] + v['y'], u['z'] + v['z']);
}
inline Vector operator-(const Vector &u, const Vector &v)
{
    return Vector(u['x'] - v['x'], u['y'] - v['y'], u['z'] - v['z']);
}
inline Vector operator*(const Vector &u, const Vector &v)
{
    return Vector(u['x'] * v['x'], u['y'] * v['y'], u['z'] * v['z']);
}
inline Vector operator*(double t, const Vector &v)
{
    return Vector(t * v['x'], t * v['y'], t * v['z']);
}
inline Vector operator*(const Vector &v, double t)
{
    return t * v;
}
inline Vector operator/(const Vector &v, double t)
{
    return (1 / t) * v;
}
inline double dot(const Vector &u, const Vector &v)
{
    return u['x'] * v['x'] + u['y'] * v['y'] + u['z'] * v['z'];
}
inline Vector cross(const Vector &u, const Vector &v)
{
    return Vector(
        u['y'] * v['z'] - u['z'] * v['y'],
        u['z'] * v['x'] - u['x'] * v['z'],
        u['x'] * v['y'] - u['y'] * v['x']);
}
inline Vector unit_vector(Vector v)
{
    return v / v.length();
}

inline Vector random_in_unit_sphere()
{
    while (true)
    {
        Vector v = Vector::random(-1, 1);
        if (v.length_squared() < 1)
            return v;
    }
}

inline Vector random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

inline Vector random_on_hemisphere(const Vector &normal)
{
    Vector on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    return -on_unit_sphere;
}

using Point = Vector;
using Color = Vector;

inline Color random_color()
{
    return Color(random_double(), random_double(), random_double());
}

class Ray
{
private:
    Point org;
    Vector dir;

public:
    Ray();
    Ray(const Point &org_, const Vector &dir_);
    Point origin() const;
    Vector direction() const;
    Point at(double x) const;
};

Ray::Ray(){};
Ray::Ray(const Point &org_, const Vector &dir_) : org(org_), dir(dir_) {}
Point Ray::origin() const
{
    return org;
}
Vector Ray::direction() const
{
    return dir;
}
Point Ray::at(double x) const
{
    return org + x * dir;
}

// main
double hit_sphere(const Point &center, double radius, double min, double max, const Ray ray)
{
    Vector OC = ray.origin() - center;
    double a = ray.direction().length_squared();
    double half_b = dot(OC, ray.direction());
    double c = OC.length_squared() - radius * radius;
    double delta = half_b * half_b - a * c;
    if (delta < 0)
        return -1.0;
    double sqrtd = sqrt(delta);

    double root = ((-half_b - sqrtd) / a);

    if (root < min || root > max)
    {
        root = (-half_b + sqrtd) / a;
        if (root < min || root > max)
            return -1;
    }

    return root;
}

enum MaterialType
{
    Metal,
    Lambertian,
    Dielectric,
};

struct Material
{
    MaterialType type;
    Color attenuation = {1, 1, 1};
    double fuzz;
    double ir;
};

struct Sphere
{
    double radius;
    Point center;
    Material *material;
};

struct World
{
    Material materials[2048];
    Sphere spheres[2048];
    int sphere_count;
    int materials_count;
};

Vector reflect(const Vector &v, const Vector &n)
{
    return v - 2 * dot(v, n) * n;
}

Vector refract(const Vector &uv, const Vector &n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    Vector r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vector r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

Color ray_color(const Ray &ray, int depth, World &world)
{
    if (depth == 50)
    {
        return Color(0, 0, 0);
    }

    // Material ground = {.type = Lambertian, .attenuation = {0.8, 0.8, 0}};
    // Material center = {.type = Lambertian, .attenuation = {0.7, 0.3, 0.3}};
    // Material left = {.type = Metal, .attenuation = {0.8, 0.8, 0.8}, .fuzz = 0.3};
    // Material right = {.type = Metal, .attenuation = {0.8, 0.6, 0.2}, .fuzz = 0};

    // Sphere spheres[] = {

    //     {.radius = 100, .center = {0, -100.5, -1}, .material = &ground},

    //     {.radius = 0.5, .center = {0, 0, -1}, .material = &center},

    //     {.radius = 0.5, .center = {-1, 0, -1}, .material = &left},
    //     {.radius = 0.5, .center = {1, 0, -1}, .material = &right},
    // };

    // int sphere_count = sizeof(spheres) / sizeof(*spheres);

    double closest = infinity;
    int hit = -1;

    for (int i = 0; i < world.sphere_count; i++)
    {
        double t = hit_sphere(world.spheres[i].center, world.spheres[i].radius, 0.001, closest, ray);

        if (t > 0 && t < closest)
        {
            hit = i;
            closest = t;
        }
    }

    if (hit != -1)
    {
        Vector p = ray.at(closest);

        Vector outward_normal = unit_vector(p - world.spheres[hit].center);
        bool front_face = dot(ray.direction(), outward_normal) < 0;
        Vector normal = front_face ? outward_normal : -outward_normal;

        Vector dir;

        // Vector dir =

        if (world.spheres[hit].material->type == Metal)
        {
            Vector ray_dir_normal = unit_vector(ray.direction());
            dir = reflect(ray_dir_normal, normal);

            dir += world.spheres[hit].material->fuzz * random_unit_vector();
        }
        else
            dir = normal + random_unit_vector();

        Ray nray = Ray(p, dir);

        return world.spheres[hit].material->attenuation * ray_color(nray, depth + 1, world);
    }

    Vector unit_direction = unit_vector(ray.direction());
    // random number !!
    double a = 0.5 * (unit_direction['y'] + 1.0);

    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1);
}

void write_color(std::ostream &out, Color color)
{
    int r = sqrt(color['r']) * 255.999;
    int g = sqrt(color['g']) * 255.999;
    int b = sqrt(color['b']) * 255.999;

    out << r << " " << g << " " << b << "\n";
};

Material *add_material(World &world, MaterialType type)
{
    Material *m = &world.materials[world.materials_count++];

    m->type = type;
    return m;
}

Sphere *add_sphere(World &world, Point center, double radius, Material *m)
{
    Sphere *s = &world.spheres[world.sphere_count++];

    s->center = center;
    s->radius = radius;
    s->material = m;

    return s;
}

inline Vector random_in_unit_disk()
{
    while (true)
    {
        auto p = Vector(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

int main(void)
{

    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 800;
    int image_height = (int)(image_width / aspect_ratio);
    if (image_height < 1)
        image_height = 1;

    // Camera

    double defocus_angle = 0.1;
    double focus_dist = 10;
    Vector defocus_disk_u; // Defocus disk horizontal radius
    Vector defocus_disk_v; // Defocus disk vertical radius

    Point lookfrom = Point(13, 2, 3); // Point camera is looking from
    Point lookat = Point(0, 0, 0);    // Point camera is looking at
    Vector vup = Vector(0, 1, 0);

    Vector u, v, w;

    double vfov = 20;

    // double focal_length = (lookfrom - lookat).length();

    auto theta = degrees_to_radians(vfov);

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    auto viewport_height = 2 * tan(theta * 0.5) * focus_dist;

    // double viewport_height = 2.0;
    double viewport_width = viewport_height * ((double)image_width / image_height);
    Point camera_center = lookfrom;

    // view port vectors (u, v)
    Vector viewport_u = viewport_width * u;
    Vector viewport_v = -viewport_height * v;

    // pixel vectors
    Vector pixel_u = viewport_u / image_width;
    Vector pixel_v = viewport_v / image_height;

    // first pixel

    auto viewport_upper_left = camera_center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    Point first_pixel = viewport_upper_left + 0.5 * (pixel_u + pixel_v);

    // Point first_pixel = camera_center - Vector(0, 0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0 + 0.5 * (pixel_u + pixel_v);

    auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;

    World world = {};

    Material *m = add_material(world, Lambertian);

    m->attenuation = Color(0.5, 0.5, 0.5);

    add_sphere(world, Point(0, -1000, 0), 1000, m);

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = random_double();
            Point center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Point(4, 0.2, 0)).length() > 0.9)
            {

                Material *sphere_material;

                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo = random_color() * random_color();
                    sphere_material = add_material(world, Lambertian);

                    sphere_material->attenuation = albedo;
                }
                else
                {
                    // metal
                    auto albedo = random_color();
                    auto fuzz = random_double(0, 0.5);

                    sphere_material = add_material(world, Metal);
                    sphere_material->attenuation = albedo;
                }
                add_sphere(world, center, 0.2, sphere_material);
            }
        }
    }

    m = add_material(world, Lambertian);
    m->attenuation = Color(0.4, 0.2, 0.1);
    add_sphere(world, Point(-4, 1, 0), 1, m);

    m = add_material(world, Metal);
    m->fuzz = 0;
    m->attenuation = Color(0.7, 0.6, 0.5);

    add_sphere(world, Point(4, 1, 0), 1, m);

    // Render
    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";
    for (int h = 0; h < image_height; h++)
    {
        std::clog << "\rScanlines remaining: " << (image_height - h) << ' ' << std::flush;
        for (int w = 0; w < image_width; w++)
        {
            Point pixel_center = first_pixel + (w * pixel_u) + (h * pixel_v);

            int rays_per_pixel = 512;
            Color final_color(0, 0, 0);

            for (int i = 0; i < rays_per_pixel; i++)
            {
                double x = random_double(-0.5, 0.5);
                double y = random_double(-0.5, 0.5);

                Point pixel = pixel_center + x * pixel_u + y * pixel_v;

                Vector ray_origin;

                if (defocus_angle <= 0)
                    ray_origin = camera_center;
                else
                {
                    auto p = random_in_unit_disk();
                    ray_origin = camera_center + (p['x'] * defocus_disk_u) + (p['y'] * defocus_disk_v);
                }

                Vector ray_direction = pixel - ray_origin;
                // std::cout << ray_direction << std::endl;
                Ray ray(camera_center, ray_direction);
                final_color += ray_color(ray, 0, world);
            }
            final_color /= rays_per_pixel;
            write_color(std::cout, final_color);
        }
    }
}