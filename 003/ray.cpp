#include "color.cpp"

class Ray
{
private:
    Point org;
    Vector dir;

public:
    Ray();
    Ray(Point &org_, Vector &dir_);
    Point origin() const;
    Vector direction() const;
};

Ray::Ray(){};
Ray::Ray(Point &org_, Vector &dir_) : org(org_), dir(dir_){};
Point Ray::origin() const
{
    return org;
}
Vector Ray::direction() const
{
    return dir;
}
