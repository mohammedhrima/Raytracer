#include "header.hpp"

Vec3 calc(Vec3 l, char c, Vec3 r)
{
    if (c == '+')
        return ((Vec3){.x = l.x + r.x, .y = l.y + r.y, .z = l.z + r.z});
    if (c == '-')
        return ((Vec3){.x = l.x - r.x, .y = l.y - r.y, .z = l.z - r.z});
    if (c == '*')
        return ((Vec3){.x = l.x * r.x, .y = l.y * r.y, .z = l.z * r.z});
    if (c == '/')
    {
        if (r.x == 0.0 || r.y == 0.0 || r.z == 0.0)
        {
            printf("Error 2: dividing by 0\n");
            exit(1);
        }
        return ((Vec3){.x = l.x / r.x, .y = l.y / r.y, .z = l.z / r.z});
    }

    return ((Vec3){});
}

Vec3 scale(Vec3 v, char c, float t)
{
    if (c == '*')
        return ((Vec3){.x = t * v.x, .y = t * v.y, .z = t * v.z});
    if (c == '/')
    {
        if (t == 0)
        {
            printf("Error 1: dividing by 0\n");
            exit(1);
        }
        return ((Vec3){.x = v.x / t, .y = v.y / t, .z = v.z / t});
    }
    return ((Vec3){});
}

// vectors utils
float degrees_to_radians(float degrees)
{
    return degrees * PI / 180.0;
}
float pow2(float x)
{
    return x * x;
}
// thank you cpp
Vec3 operator+(Vec3 l, Vec3 r)
{
    return (Vec3){l.x + r.x, l.y + r.y, l.z + r.z};
}
Vec3 operator-(Vec3 l, Vec3 r)
{
    return (Vec3){l.x - r.x, l.y - r.y, l.z - r.z};
}
Vec3 operator*(Vec3 l, Vec3 r)
{
    return (Vec3){l.x * r.x, l.y * r.y, l.z * r.z};
}
Vec3 operator*(float t, Vec3 v)
{
    return (Vec3){t * v.x, t * v.y, t * v.z};
}
Vec3 operator*(Vec3 v, float t)
{
    return t * v;
}
Vec3 operator/(Vec3 v, float t)
{
    if (t == 0)
    {
        printf("Error 1: dividing by 0\n");
        exit(1);
    }
    return (Vec3){v.x / t, v.y / t, v.z / t};
}
std::ostream &operator<<(std::ostream &out, Vec3 &v)
{
    out << "( " << v.x << ", " << v.y << ", " << v.z << ")";
    return out;
}
float length_squared(Vec3 v)
{
    return pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2);
}
float length(Vec3 v)
{
    return sqrt(length_squared(v));
}
float dot(Vec3 u, Vec3 v)
{
    return (u.x * v.x + u.y * v.y + u.z * v.z);
}

Vec3 cross(Vec3 u, Vec3 v)
{
    return (Vec3){u.y * v.z - u.z * v.y,
                  u.z * v.x - u.x * v.z,
                  u.x * v.y - u.y * v.x};
}

Vec3 unit_vector(Vec3 v)
{
    float f = length(v);

    if (f <= ZERO)
        return (Vec3){};
    return v / f;
}

static unsigned rng_state;
static const double one_over_uint_max = 1.0 / UINT_MAX;
unsigned rand_pcg()
{
    unsigned state = rng_state;
    rng_state = rng_state * 747796405u + 2891336453u;
    unsigned word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float random_float(float min, float max)
{
    return min + (rand_pcg() * one_over_uint_max) * (max - min);
}
Vec3 random_unit_vector()
{
    Vec3 u;
    while (1)
    {
        u = (Vec3){random_float(-1, 1), random_float(-1, 1), random_float(-1, 1)};
        if (length_squared(u) <= 1)
            break;
    }
    Vec3 v = unit_vector(u);
    return v;
}
Vec3 point_at(Ray *ray, float t)
{
    return (Vec3){ray->org.x + t * ray->dir.x, ray->org.y + t * ray->dir.y, ray->org.z + t * ray->dir.z};
}
time_t get_time()
{
    struct timespec time_;
    clock_gettime(CLOCK_MONOTONIC, &time_);
    return (time_.tv_sec * 1000 + time_.tv_nsec / 1000000);
}

void translate(Win *win, Vec3 move)
{
    Vec3 x = move.x * win->scene.u;
    Vec3 y = move.y * win->scene.v;
    Vec3 z = move.z * win->scene.w;
    win->scene.camera = win->scene.camera + x + y + z;
}

Vec3 rotate(Vec3 u, int axes, float angle)
{
    float cos_ = cos(angle);
    float sin_ = sin(angle);
    switch (axes)
    {
    case 'x':
    {
        return (Vec3){
            u.x,
            u.y * cos_ - u.z * sin_,
            u.y * sin_ + u.z * cos_};
    }
    case 'y':
    {
        return (Vec3){
            u.x * cos_ + u.z * sin_,
            u.y,
            -u.x * sin_ + u.z * cos_,
        };
    }
    case 'z':
    {
        return (Vec3){
            u.x * cos_ - u.y * sin_,
            u.x * sin_ + u.y * cos_,
            u.z};
    }
    default:
    {
        std::cerr << "Error in rotation" << std::endl;
        exit(0);
    }
    }
    return (Vec3){};
}