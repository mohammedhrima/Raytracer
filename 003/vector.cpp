#include <iostream>
#include <cmath>

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
    double Length() const;
    double Length_squared() const;
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
double Vector::Length() const
{
    return std::sqrt(Length_squared());
}
double Vector::Length_squared() const
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
    return v / v.Length();
}