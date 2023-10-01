#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

class Vector
{
private:
    int x;
    int y;
    int z;

public:
    Vector();
    Vector(int _x, int _y, int _z);
    Vector(Vector &copy);
    ~Vector();
    float get_x() const;
    float get_y() const;
    float get_z() const;
    void set_x(float value);
    void set_y(float value);
    void set_z(float value);
    float dot_product(Vector &other);
    float magnitude();
    Vector normalize();
    Vector operator+(Vector &other);
    Vector operator-(Vector &other);
    Vector operator*(int scalar);
    // Vector operator*(int scalar, Vector &v);
    Vector operator*(float scalar);
    Vector operator/(int scalar);
    Vector operator/(float scalar);
    Vector &operator=(Vector &assign);
};
std::ostream &operator<<(std::ostream &, const Vector &);

std::ostream &operator<<(std::ostream &out, const Vector &v)
{
    out << v.get_x() <<  std::setw(4)  << v.get_y() <<  std::setw(4) << v.get_z() << " ";
    return out;
};

Vector::Vector() : x(0.0), y(0.0), z(0.0) {}
Vector::Vector(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
Vector::Vector(Vector &copy)
{
    if (this != &copy)
    {
#if 0
        x = copy.x;
        y = copy.y;
        z = copy.z;
#else
        *this = copy;
#endif
    }
}
Vector::~Vector() {}

float Vector::get_x() const
{
    return x;
}

float Vector::get_y() const
{
    return y;
}

float Vector::get_z() const
{
    return z;
}

void Vector::set_x(float value)
{
    x = value;
}
void Vector::set_y(float value)
{
    y = value;
}
void Vector::set_z(float value)
{
    z = value;
}

float Vector::dot_product(Vector &other)
{
    return x * other.x + y * other.y + z * other.z;
}

float Vector::magnitude()
{
    return (float)sqrt(dot_product(*this));
}

Vector Vector::normalize()
{
    float mag = magnitude();
    return Vector(x / mag, y / mag, z / mag);
}

Vector Vector::operator+(Vector &other)
{
    return Vector(x + other.x, y + other.y, z + other.z);
}

Vector Vector::operator-(Vector &other)
{
    return Vector(x - other.x, y - other.y, z - other.z);
}

Vector Vector::operator*(int scalar)
{
    return Vector(scalar * x, scalar * y, scalar * z);
}

Vector Vector::operator*(float scalar)
{
    return Vector(scalar * x, scalar * y, scalar * z);
}

Vector operator*(int scalar, Vector &v)
{
    return v * scalar;
}

Vector operator*(float scalar, Vector &v)
{
    return v * scalar;
}

Vector Vector::operator/(int scalar)
{
    return Vector(x / scalar, y / scalar, z / scalar);
}

Vector Vector::operator/(float scalar)
{
    return Vector(x / scalar, y / scalar, z / scalar);
}

Vector operator/(float scalar, Vector &v)
{
    return v / scalar;
}

Vector operator/(int scalar, Vector &v)
{
    return v / scalar;
}

Vector &Vector::operator=(Vector &assign)
{
    if (this != &assign)
    {
        x = assign.x;
        y = assign.y;
        z = assign.z;
    }
    return *this;
}

// int main()
// {
//     Vector a(1.0, 2.0, 3.0);
//     Vector b(4.0, 5.0, 6.0);
//     Vector c = a + b;
//     Vector d = a - b;
//     Vector e = 2 * a;
//     Vector f = a * 2;

//     std::cout << "a: " << a << std::endl;
//     std::cout << "b: " << b << std::endl;
//     std::cout << "c: " << c << std::endl;
//     std::cout << "d: " << d << std::endl;
//     std::cout << "e: " << e << std::endl;
//     std::cout << "f: " << f << std::endl;
// }

