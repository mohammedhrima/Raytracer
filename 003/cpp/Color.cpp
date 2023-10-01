#include "Image.cpp"

class Color : public Vector
{
public:
    float get_r() const;
    float get_g() const;
    float get_b() const;
    void set_r(float value);
    void set_g(float value);
    void set_b(float value);
    Color();
    Color(int x_, int y_, int z_);
    ~Color();
    static Vector from_hex(std::string hex)
    {
        int x_ = std::stoi(hex.substr(0, 2), nullptr, 16);
        int y_ = std::stoi(hex.substr(2, 2), nullptr, 16);
        int z_ = std::stoi(hex.substr(4, 2), nullptr, 16);
        return Vector(x_, y_, z_);
    }
};

Color::Color() : Vector() {}
Color::Color(int x_, int y_, int z_) : Vector(x_, y_, z_) {}
Color::~Color() {}
float Color::get_r() const
{
    return get_x();
}
float Color::get_g() const
{
    return get_y();
}
float Color::get_b() const
{
    return get_z();
}
void Color::set_r(float value)
{
    set_x(value);
}
void Color::set_g(float value)
{
    set_y(value);
}
void Color::set_b(float value)
{
    set_z(value);
}