#include <cmath>
#include <iostream>
#include <limits.h>
#include <float.h>
#include <cstdlib>
#define BUG 0

const double pi = 3.1415926535897932385;
double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

double random_double()
{
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

using std::sqrt;

class Coor
{
public:
    union
    {
        struct
        {
            float x, y, z;
        };
        struct
        {
            float r, g, b;
        };
    };
    Coor()
    {
        x = 0, y = 0, z = 0;
    }
    Coor(double x_, double y_, double z_)
    {
        x = x_, y = y_, z = z_;
    }
    Coor operator-() const { return Coor(-x, -y, -z); }
    Coor &operator+=(const Coor &v)
    {
        x += v.x, y += v.y, z += v.z;
        return *this;
    }
    Coor &operator*=(double t)
    {
        x *= t, y *= t, z *= t;
        return *this;
    }
    Coor &operator/=(double t)
    {
        return *this *= 1 / t;
    }
    double length() const
    {
        return sqrt(length_squared());
    }
    double length_squared() const
    {
        return x * x + y * y + z * z;
    }
};

std::ostream &operator<<(std::ostream &out, const Coor &v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

Coor operator+(const Coor &u, const Coor &v)
{
    return Coor(u.x + v.x, u.y + v.y, u.z + v.z);
}

Coor operator-(const Coor &u, const Coor &v)
{
    return Coor(u.x - v.x, u.y - v.y, u.z - v.z);
}

Coor operator*(const Coor &u, const Coor &v)
{
    return Coor(u.x * v.x, u.y * v.y, u.z * v.z);
}

Coor operator*(double t, const Coor &v)
{
    return Coor(t * v.x, t * v.y, t * v.z);
}

Coor operator*(const Coor &v, double t)
{
    return t * v;
}

Coor operator/(Coor v, double t)
{
    return (1 / t) * v;
}

double dot(const Coor &u, const Coor &v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Coor cross(const Coor &u, const Coor &v)
{
    return Coor(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}

Coor unit_vector(Coor v)
{
    return v / v.length();
}

Coor random_vector()
{
    return Coor(random_double(), random_double(), random_double());
}

Coor random_vector(double min, double max)
{
    return Coor(random_double(min, max), random_double(min, max), random_double(min, max));
}

// ?
Coor random_in_unit_sphere()
{
    while (true)
    {
        Coor v = random_vector(-1, 1);
        if (v.length_squared() < 1)
            return v;
    }
}
// ?
Coor random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}
// ?
Coor random_on_hemisphere(Coor &norm)
{
    Coor unit_sphere = random_unit_vector();
    if (dot(unit_sphere, norm) > 0.0)
        return unit_sphere;
    return -unit_sphere;
}

typedef Coor Color;


void write_color(Color color)
{
    double x = color.x;
    double y = color.y;
    double z = color.z;

    std::cout << (int)(255.999 * sqrt(color.x)) << ' '
              << (int)(255.999 * sqrt(color.y)) << ' '
              << (int)(255.999 * sqrt(color.z)) << '\n';
}

class Ray
{
public:
    Coor dir;
    Coor org;
    Ray(){};
    Ray(Coor org_, Coor dir_)
    {
        org = org_;
        dir = dir_;
    };
    Coor at(double t)
    {
        return org + t * dir;
    }
};