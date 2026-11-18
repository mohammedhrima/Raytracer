#ifndef VEC3_HPP
#define VEC3_HPP

#include <algorithm>
#include <cmath>
#include <ostream>

struct Vec3
{
    float x;
    float y;
    float z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    Vec3 &operator+=(const Vec3 &o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }
};

typedef Vec3 Color;

inline Vec3 operator+(const Vec3 &a, const Vec3 &b) { return Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline Vec3 operator-(const Vec3 &a, const Vec3 &b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vec3 operator*(const Vec3 &a, const Vec3 &b) { return Vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline Vec3 operator*(float t, const Vec3 &v) { return Vec3(t * v.x, t * v.y, t * v.z); }
inline Vec3 operator*(const Vec3 &v, float t) { return t * v; }
inline Vec3 operator/(const Vec3 &v, float t) { return (1.0f / t) * v; }

inline float dot(const Vec3 &a, const Vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float lengthSquared(const Vec3 &v) { return dot(v, v); }
inline float length(const Vec3 &v) { return std::sqrt(lengthSquared(v)); }

inline Vec3 cross(const Vec3 &a, const Vec3 &b)
{
    return Vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}

inline Vec3 normalize(const Vec3 &v)
{
    float len = length(v);
    if (len < 1e-8f)
        return v;
    return v / len;
}

inline Vec3 reflect(const Vec3 &v, const Vec3 &n)
{
    return v - 2.0f * dot(v, n) * n;
}

// Snell's law. dir and n are unit vectors, n points against dir,
// eta = n_from / n_to.
inline Vec3 refract(const Vec3 &dir, const Vec3 &n, float eta)
{
    float cosTheta = std::min(dot(-dir, n), 1.0f);
    Vec3 perpendicular = eta * (dir + cosTheta * n);
    Vec3 parallel = -std::sqrt(std::fabs(1.0f - lengthSquared(perpendicular))) * n;
    return perpendicular + parallel;
}

inline Vec3 rotateX(const Vec3 &v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Vec3(v.x, v.y * c - v.z * s, v.y * s + v.z * c);
}

inline Vec3 rotateY(const Vec3 &v, float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Vec3(v.x * c + v.z * s, v.y, -v.x * s + v.z * c);
}

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v)
{
    return out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

struct Ray
{
    Vec3 origin;
    Vec3 direction;

    Ray() {}
    Ray(const Vec3 &o, const Vec3 &d) : origin(o), direction(d) {}

    Vec3 at(float t) const { return origin + t * direction; }
};

#endif
