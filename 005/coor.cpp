#include <cmath>
#include <iostream>
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

using Color = Coor;
void write_color(Color color)
{
    std::cout << (int)(255.999 * color.x) << ' '
              << (int)(255.999 * color.y) << ' '
              << (int)(255.999 * color.z) << '\n';
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