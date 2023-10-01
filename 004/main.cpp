#include <iostream>

#define EPSILON 0.0001

class Tuple
{
private:
    float x;
    float y;
    float z;
    float w;

public:
    Tuple() : x(0.0), y(0.0), z(0.0), w(0.0){};
    Tuple(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_){};
    ~Tuple(){};
    bool operator==(Tuple &other);
    Tuple &operator+(Tuple &other);
};

Tuple &Tuple::operator+(Tuple &other)
{
    // TODO: w to be checked
    Tuple c(x + other.x, y + other.y, z + other.z, w);
    return c;
}

bool Tuple::operator==(Tuple &other)
{
    // TODO: to be check it negative ...
    if (x - other.x < EPSILON && y - other.y < EPSILON && z - other.z < EPSILON)
        return true;
    return (false);
}

Tuple Point(float x_, float y_, float z_)
{
    return Tuple(x_, y_, z_, 1.0);
}

Tuple Vector(float x_, float y_, float z_)
{
    return Tuple(x_, y_, z_, 0.0);
}

int main()
{
    Tuple point(4.3, -4.2, 3.1, 1.0);
    Tuple vector(4.3, -4.2, 3.1, 0.0);
}
