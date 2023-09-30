#include <iostream>
#include <cmath>

class Vector
{
public:
    float x, y;
    Vector(){};
    Vector(float X, float Y)
    {
        x = X;
        y = Y;
    };
    float Length() const
    {
        float length;
        length = sqrt(x * x + y * y);
        return length;
    };
    float LengthSqrt() const
    {
        float length;
        length = x * x + y * y;
        return length;
    };
    Vector Normalized() const // get ^i (i hat)
    {
        Vector normalized;
        normalized = (*this) / Length();
        return normalized;
    };
    Vector operator+(const Vector &v) const
    {
        return Vector(x + v.x, y + v.y);
    };
    Vector operator-(const Vector &v) const
    {
        return Vector(x - v.x, y - v.y);
    };
    Vector operator*(float s) const
    {
        return Vector(x * s, y * s);
    };
    Vector operator/(float s) const
    {
        return Vector(x / s, y / s);
    };
};

class Point
{
public:
    float x, y;
    Point(){};
    Point(float X, float Y)
    {
        x = X;
        y = Y;
    };
    Point AddVector(Vector v)
    {
        return Point(x + v.x, y + v.y);
    };
};

Vector operator-(Point a, Point b)
{
    return Vector(a.x - b.x, a.y - b.y);
}

Vector operator+(Point a, Point b)
{
    return Vector(a.x + b.x, a.y + b.y);
}

int main()
{
    Point r(4, 0);
    Point d(0, -5);

    Vector v = r + d;

    std::cout << "Pac man's new velocity: (" << v.x << ", " << v.y << ")\n";
}