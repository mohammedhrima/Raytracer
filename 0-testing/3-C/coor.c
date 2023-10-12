#include "header.h"


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
#if 1
    return min + (rand_pcg() * one_over_uint_max) * (max - min);
#else
    return min + ((double)rand() / RAND_MAX) * (max - min);
#endif
}

const float pi = 3.1415926535897932385;

float degrees_to_radians(float degrees)
{
    return degrees * pi / 180.0;
}

// float random_double1()
// {
//     return rand() / (RAND_MAX + 1.0);
// }
// float random_float(float min, float max)
// {
//     return min + (max - min) * random_double1();
// }

Coor new_coor(float x, float y, float z)
{
    // Coor *new = calloc(1, sizeof(Coor));
    // new->x = x;
    // new->y = y;
    // new->z = z;
    // return new;
    return (Coor){.x = x, .y = y, .z = z};
}


Color new_color(float r, float g, float b)
{
    // Coor *new = calloc(1, sizeof(Color));
    // new->r = r;
    // new->g = g;
    // new->b = b;
    return (Color){.r = r /255.999, .g = g/255.999, .b = b/255.999};
}

// Color rgb()
// {

// }

Coor add_(Coor l, Coor r)
{
    return (Coor){.x = l.x + r.x, .y = l.y + r.y, .z = l.z + r.z};
}

Coor sub_(Coor l, Coor r)
{
    return (Coor){.x = l.x - r.x, .y = l.y - r.y, .z = l.z - r.z};
}

Coor neg_(Coor v)
{
    return (Coor){.x = -v.x, .y = -v.y, .z = -v.z};
}

Coor mul_(float t, Coor v)
{
    return new_coor(t * v.x, t * v.y, t * v.z);
}

Coor mul(Coor leftv, Coor rightv)
{
    return new_coor(leftv.x * rightv.x, leftv.y * rightv.y, leftv.z * rightv.z);
}

Coor div_(Coor v, float t)
{
    return mul_(1 / t, v);
}

float length_squared(Coor v)
{
    return pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2);
}

float length(Coor v)
{
    return sqrt(length_squared(v));
}

float dot(Coor u, Coor v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Coor unit_vector(Coor v)
{
    return div_(v, length(v));
}

Coor random_vector(float min, float max)
{
    return new_coor(random_float(min, max), random_float(min, max), random_float(min, max));
}

Coor random_in_unit_sphere()
{
    while (1)
    {
        Coor v = random_vector(-1, 1);
        if (length_squared(v) < 1)
            return v;
        // free(v);
    }
}

Coor random_unit_vector()
{
    Coor u = random_in_unit_sphere();
    Coor v = unit_vector(u);
    // free(u);
    return v;
}

// void write_color(Color *color)
// {
//     int x = (int)(255.999 * sqrt(color->x));
//     int y = (int)(255.999 * sqrt(color->y));
//     int z = (int)(255.999 * sqrt(color->z));
//     printf("%d %d %d ", x, y, z);
// }

Ray new_ray(Coor org, Coor dir)
{
    // Ray *new = calloc(1, sizeof(Ray));
    // new->dir = dir;
    // new->org = org;
    return (Ray){.dir = dir, .org = org};
}

void free_ray(Ray *ray)
{
    free(ray);
}

Coor point_at(Ray ray, float t)
{
    return (Coor){.x = ray.org.x + t * ray.dir.x, .y = ray.org.y + t * ray.dir.y, .z = ray.org.z + t * ray.dir.z};
}