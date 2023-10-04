#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <cstdlib>

// using std::make_shared;
// using std::shared_ptr;
using std::sqrt;
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}
inline double random_double()
{
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

class Vector
{
public:
    double e[3];
    // constractors
    Vector();
    Vector(double e0, double e1, double e2);
    // getters
    double x() const;
    double y() const;
    double z() const;
    // operators overloading
    Vector operator-() const;
    double operator[](int i) const;
    Vector &operator+=(const Vector &v);
    Vector &operator*=(double t);
    Vector &operator/=(double t);
    // methods
    double length() const;
    double length_squared() const;
};

Vector::Vector() : e{0, 0, 0} {};
Vector::Vector(double e0, double e1, double e2) : e{e0, e1, e2} {};
double Vector::x() const
{
    return e[0];
}
double Vector::y() const
{
    return e[1];
}
double Vector::z() const
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
    return sqrt(length_squared());
}
double Vector::length_squared() const
{
    return (e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
}
inline std::ostream &operator<<(std::ostream &out, const Vector &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}
inline Vector operator+(const Vector &u, const Vector &v)
{
    return Vector(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}
inline Vector operator-(const Vector &u, const Vector &v)
{
    return Vector(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}
inline Vector operator*(const Vector &u, const Vector &v)
{
    return Vector(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}
inline Vector operator*(double t, const Vector &v)
{
    return Vector(t * v.x(), t * v.y(), t * v.z());
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

using Point = Vector;
using Color = Vector;

// ray
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
    Interval(double _min, double _max);
    bool contains(double x) const;
    bool surrounds(double x) const;
    double clamp(double x) const;
    static const Interval empty;
    static const Interval universe;
};

Interval::Interval() : min(+infinity), max(-infinity){};
Interval::Interval(double _min, double _max) : min(_min), max(_max){};
bool Interval::contains(double x) const
{
    return x >= min && x <= max;
}
bool Interval::surrounds(double x) const
{
    return x > min && x < max;
}
double Interval::clamp(double x) const
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}
const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);

// hit_record
class Hit_record
{
public:
    Point point;
    Vector normal;
    double x;
    bool front_face;

    void set_face_normal(const Ray &ray, const Vector &outward_normal);
};

void Hit_record::set_face_normal(const Ray &ray, const Vector &outward_normal)
{
    front_face = dot(ray.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
};

// hittable
class Hittable
{
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray &ray, Interval limits, Hit_record &rec) const = 0;
};

// Sphere
class Sphere : public Hittable
{
private:
    Point center;
    double radius;

public:
    Sphere(Point center_, double radius_);
    bool hit(const Ray &rar, Interval limits, Hit_record &rec) const;
};

Sphere::Sphere(Point center_, double radius_) : center(center_), radius(radius_){};

bool Sphere::hit(const Ray &ray, Interval limits, Hit_record &rec) const
{
    Vector OC = ray.origin() - center;
    double a = ray.direction().length_squared();
    double half_b = dot(OC, ray.direction());
    double c = OC.length_squared() - radius * radius;
    double delta = half_b * half_b - a * c;
    if (delta < 0)
        return false;
    // solution
    double x1 = (-half_b - sqrt(delta)) / a;
    double x2 = (-half_b + sqrt(delta)) / a;
#if 0
    if  (x1 > limits.min && x1 < limits.max)
        rec.x = x1;
    else if (x2 > limits.min && x2 < limits.max)
        rec.x = x2;
    else
        return false;
#else
    if (limits.surrounds(x1))
        rec.x = x1;
    else if (limits.surrounds(x2))
        rec.x = x2;
    else
        return false;
#endif

    rec.point = ray.at(rec.x);
    Vector outward_normal = (rec.point - center) / radius; // TODO: to be checked ??
    rec.set_face_normal(ray, outward_normal);
    return true;
}

// hittable list
class Hittable_list : public Hittable
{
public:
    std::vector<Hittable *> objects;
    Hittable_list();
    Hittable_list(Hittable &object);
    void clear();
    void add(Hittable *object);
    bool hit(const Ray &ray, Interval limits, Hit_record &rec) const;
};

Hittable_list::Hittable_list(){};
Hittable_list::Hittable_list(Hittable &object)
{
    add(&object);
}
void Hittable_list::add(Hittable *object)
{
    objects.push_back(object);
}
void Hittable_list::clear()
{
    objects.clear();
}
bool Hittable_list::hit(const Ray &ray, Interval limits, Hit_record &rec) const
{
    Hit_record tmp;
    bool did_hit_something = false;
    double closest = limits.max;

    for (const auto &object : objects)
    {
        if (object->hit(ray, Interval(limits.min, closest), tmp))
        {
            did_hit_something = true;
            closest = tmp.x;
            rec = tmp;
        }
    }
    return did_hit_something;
};

// Camera
class Camera
{
    // Camera is the origin O(0,0,0)
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 100;

    void render(const Hittable &world);

private:
    int image_height;
    Point camera_center;
    Point first_pixel;
    Vector pixel_u;
    Vector pixel_v;
    void initialize();
    Color ray_color(const Ray &ray, const Hittable &world) const;
    Ray get_ray(int h, int w);
    Vector pixel_sample_square() const;
};

void write_color(std::ostream &out, Color pixel, int samples_per_pixel)
{
#if 1
    double scale = 1.0 / samples_per_pixel;
    double r = pixel.x() * scale;
    double g = pixel.y() * scale;
    double b = pixel.z() * scale;

    static const Interval itensity(0.000, 0.999);
    out << (int)(256 * itensity.clamp(r)) << " " << (int)(256 * itensity.clamp(g)) << " " << (int)(256 * itensity.clamp(b)) << " ";
#else
    out << (int)(255 * pixel.x()) << " " << (int)(255 * pixel.y()) << " " << (int)(256 * pixel.z()) << " ";
#endif
}

Vector Camera::pixel_sample_square() const
{
    double px = -0.5 + random_double();
    double py = -0.5 + random_double();
    return (px * pixel_u + py * pixel_v);
}

Ray Camera::get_ray(int w, int h)
{
    Point pixel_center = first_pixel + (h * pixel_v) + (w * pixel_u);
    Point pixel_sample = pixel_center + pixel_sample_square();

    Vector ray_direction = pixel_sample - camera_center;
    return Ray(camera_center, ray_direction);
}

Color Camera::ray_color(const Ray &ray, const Hittable &world) const
{
    // draw circle on viewport
    // the sphere is in position (0,0,-1) , radius 0.5

    // Point sphere_center = Point(0, 0, -1);
    // double x = hit_sphere(sphere_center, 0.5, ray);
    Hit_record rec;

#if 0
    if (world.hit(ray, Interval(0, infinity), rec))
        return Color(1, 0, 0);
    Vector vector_unit = unit_vector(ray.direction());
    double a = 0.5 * (vector_unit.y() + 1.0);
    return a * Color(1, 1, 1);
#else
    if (world.hit(ray, Interval(0, infinity), rec))
        return 0.5 * (rec.normal + Color(1, 1, 1));
    Vector vector_unit = unit_vector(ray.direction());
    double a = 0.5 * (vector_unit.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
#endif
}

void Camera::render(const Hittable &world)
{
    initialize();
    std::cout << "P3\n"
              << image_width << " "
              << image_height << "\n255\n";
    // iterate over each pixel in viewport
    for (int h = 0; h < image_height; h++)
    {
        for (int w = 0; w < image_width; w++)
        {
#if 1
            Color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++)
            {
                Point pixel_center = first_pixel + (h * pixel_v) + (w * pixel_u);
                Point pixel_sample = pixel_center + pixel_sample_square();
                Vector ray_direction = pixel_sample - camera_center;
                Ray ray(camera_center, ray_direction);
                pixel_color += ray_color(ray, world);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
            std::cout << "\n";
#else
            Color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++)
            {
                Ray ray = get_ray(w, h);
                pixel_color += ray_color(ray, world);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
            std::cout << "\n";
#endif
        }
    }
    std::clog << "\rDone\n";
}

void Camera::initialize()
{
    image_height = (int)(image_width / aspect_ratio);
    if (image_height < 1)
        image_height = 1;

    // viewport is the screen
    double focal_length = 1.0; // distance between camera and viewport
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double)(image_width) / image_height;

    // viewport vectors
    Vector viewport_u = Vector(viewport_width, 0, 0);
    Vector viewport_v = Vector(0, -viewport_height, 0);

    // vertical and horizontal distance between each pixel in viewport
    pixel_u = viewport_u / image_width;
    pixel_v = viewport_v / image_height;

    // upper left pixel coordinates (the center fo pixel)
    first_pixel = camera_center - Vector(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2 + 0.5 * (pixel_u + pixel_v);
}

int main(void)
{
    // World
    Hittable_list world;
    Sphere sphere1 = Sphere(Point(0, 0, -1), 0.5);
    Sphere sphere2 = Sphere(Point(0, -100.5, -1), 100);

    world.add(&sphere1);
    world.add(&sphere2);

    Camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.render(world);
}
