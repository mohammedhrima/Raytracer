#include <iostream>
#include <cmath>
#include <stdbool.h>
#include <memory>
#include <vector>
#include <limits>

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

// Vector class
class Vector
{
public:
    double e[3];
    Vector();
    Vector(double, double, double);
    double get_x() const;
    double get_y() const;
    double get_z() const;
    Vector operator-() const;
    double operator[](int) const;
    double &operator[](int);
    Vector &operator+=(const Vector &);
    Vector &operator*=(double);
    Vector &operator/=(double);
    double length() const;
    double length_squared() const;
};

Vector::Vector() : e{0, 0, 0} {};
Vector::Vector(double e1, double e2, double e3) : e{e1, e2, e3} {};
double Vector::get_x() const
{
    return e[0];
}
double Vector::get_y() const
{
    return e[1];
}
double Vector::get_z() const
{
    return e[2];
}
Vector Vector::operator-() const
{
    return Vector(-e[0], -e[1], -e[2]);
}
double Vector::operator[](int i) const
{
    return e[i];
}
double &Vector::operator[](int i)
{
    return e[i];
}
Vector &Vector::operator+=(const Vector &v)
{
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}
Vector &Vector::operator*=(double t)
{
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}
Vector &Vector::operator/=(double t)
{
    return *this *= 1 / t;
}
double Vector::length() const
{
    return std::sqrt(length_squared());
}
double Vector::length_squared() const
{
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

using Point = Vector;

inline std::ostream &operator<<(std::ostream &out, const Vector &v)
{
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2];
}
inline Vector operator+(const Vector &u, const Vector &v)
{
    return Vector(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}
inline Vector operator-(const Vector &u, const Vector &v)
{
    return Vector(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}
inline Vector operator*(const Vector &u, const Vector &v)
{
    return Vector(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}
inline Vector operator*(double t, const Vector &v)
{
    return Vector(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline Vector operator*(Vector v, double t)
{
    return t * v;
}
inline Vector operator/(Vector v, double t)
{
    return (1 / t) * v;
}
inline double dot(const Vector &u, const Vector &v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}
inline Vector cross(const Vector &u, const Vector &v)
{
    return Vector(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                  u.e[2] * v.e[0] - u.e[0] * v.e[2],
                  u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
inline Vector unit_vector(Vector v)
{
    return v / v.length();
}

// Color class
using Color = Vector;
void write_color(std::ostream &out, Color pixel)
{
    out << (int)(255.999 * pixel.get_x()) << " "
        << (int)(255.999 * pixel.get_y()) << " "
        << (int)(255.999 * pixel.get_z()) << "\n";
}

// Ray class
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
    Point at(double t) const;
};

Ray::Ray() {}
Ray::Ray(const Point &org_, const Point &dir_) : org(org_), dir(dir_) {}
Point Ray::origin() const
{
    return org;
}
Vector Ray::direction() const
{
    return dir;
}
Point Ray::at(double t) const
{
    return org + t * dir;
}

// Interval
class Interval
{
public:
    double min;
    double max;

    Interval();
    Interval(const double _min, const double _max);
    bool contains(double x) const;
    bool surrounds(double x) const;

    static const Interval empty;
    static const Interval universe;
};

Interval::Interval() : min(+infinity), max(-infinity) {}
Interval::Interval(const double _min, const double _max) : min(_min), max(_max) {}
bool Interval::contains(double x) const
{
    return x <= max && x >= min;
}
bool Interval::surrounds(double x) const
{
    return x < max && x > min;
}
const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);

// Hittable
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

// Hittable list
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

// Sphere
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

// Camera
class Camera
{
public:
    double aspect_ratio = 1.0;
    int width = 100;

private:
    int height;
    Point camera_center;
    Point pixel00_center;
    Vector pixel_delta_u;
    Vector pixel_delta_v;

    // methods
    void initialize();
    Color ray_color(const Ray &ray, const Hittable &world) const;

public:
    void render(const Hittable &world);
};

void Camera::initialize()
{
    // Image
    // TODO: try testing width = height
    height = (int)(width / aspect_ratio);
    if (height < 1)
        height = 1;

    // Camera
    camera_center = Point(0, 0, 0);

    // Viewport
    double focal_length = 1.0;
    double viewport_h = 2.0;
    double viewport_w = viewport_h * (double)width / height;

    // Viewport vectors
    Vector viewport_u(viewport_w, 0, 0);
    Vector viewport_v(0, -viewport_h, 0);

    // Viewport vertical and horizontal pixel to pixel
    pixel_delta_u = viewport_u / width;
    pixel_delta_v = viewport_v / height;

    Point viewport_upper_left = camera_center - Vector(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    pixel00_center = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}

Color Camera::ray_color(const Ray &ray, const Hittable &world) const
{
    Hit_record rec;
    Interval val(0, infinity);
    if (world.hit(ray, val, rec))
        return 0.5 * (rec.normal + Color(1, 1, 1));

    Vector unit_direction = unit_vector(ray.direction());
    double a = 0.5 * (unit_direction.get_y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

void Camera::render(const Hittable &world)
{
    initialize();
    // Render
    std::cout << "P3\n"
              << width << " " << height << "\n255\n";
    for (int h = 0; h < height; h++)
    {
        std::clog << "\rScanlines remaining: " << (height - h) << ' ' << std::flush;
        for (int w = 0; w < width; w++)
        {
            Point pixel_center = pixel00_center + (w * pixel_delta_u) + (h * pixel_delta_v);
            Vector ray_direction = pixel_center - camera_center;
            Ray ray(camera_center, ray_direction);
            // Color pixel = Color((double)w / (width - 1), (double)h / (height - 1), 0.0);
            Color pixel_color = ray_color(ray, world);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";
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