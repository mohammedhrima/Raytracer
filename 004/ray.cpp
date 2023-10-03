#include "color.cpp"

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