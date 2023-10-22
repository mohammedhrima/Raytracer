#include <mlx.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ESC 53
#define UP 126
#define DOWN 125
#define LEFT 123
#define RIGHT 124
#define RED 0xff0000
#define ZERO .0001f

typedef enum
{
    Refl_ = 11,
    Refr_,
    Abs_
} Mat;

typedef enum
{
    sphere_ = 22,
    plan_,
    triangle_
} Type;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
    };
    struct
    {
        float r;
        float g;
        float b;
    };
} Vec3;

typedef Vec3 Color;

typedef struct
{
    Type type;
    Mat mat;
    Color color;
    Color light_color;
    float light_intensity;
    union
    {
        // Sphere
        struct
        {
            Vec3 center;
            float radius;
        };
        struct
        {
            Vec3 normal;
            union
            {
                // triangle
                struct
                {
                    Vec3 p1;
                    Vec3 p2;
                    Vec3 p3;
                };
                // Plan
                float d;
            };
        };
    };
} Obj;

typedef struct
{
    Vec3 dir;
    Vec3 org;
} Ray;

typedef struct
{
    // rendering
    float len;
    float view_angle;
    Vec3 camera;
    Vec3 camera_dir;
    Vec3 screen_u;
    Vec3 screen_v;
    Vec3 pixel_u;
    Vec3 pixel_v;
    Vec3 first_pixel;
    Vec3 u, v, w;
    Obj objects[100];
    int pos;
} Scene;

typedef struct
{
    int width;
    int height;
    void *mlx;
    void *win;
    // image
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    Scene scene;
} Win;

// mlx
int listen(int keycode, Win *vars)
{
    switch (keycode)
    {
    case ESC:
        mlx_destroy_window(vars->mlx, vars->win);
        exit(0);
    default:
        printf("%d\n", keycode);
    }
    return (0);
}

// utils
static unsigned rng_state;
static const double one_over_uint_max = 1.0 / UINT_MAX;
const float pi = 3.1415926535897932385;
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
float degrees_to_radians(float degrees)
{
    return degrees * pi / 180.0;
}

Color color(float r, float g, float b)
{
    return (Color){r / 255.999, g / 255.999, b / 255.999};
}
Vec3 add_vec3(Vec3 l, Vec3 r)
{
    return (Vec3){l.x + r.x, l.y + r.y, l.z + r.z};
}
Vec3 sub_vec3(Vec3 l, Vec3 r)
{
    return (Vec3){l.x - r.x, l.y - r.y, l.z - r.z};
}

Vec3 mul_vec3(float t, Vec3 v)
{
    return (Vec3){t * v.x, t * v.y, t * v.z};
}
Vec3 mul_vec3_(Vec3 l, Vec3 r)
{
    return (Vec3){l.x * r.x, l.y * r.y, l.z * r.z};
}
Vec3 div_vec3(Vec3 v, float t)
{
    if (t == 0)
    {
        printf("Error: dividing by 0\n");
        exit(1);
    }
    return mul_vec3(1 / t, v);
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
    return u.x * v.x + u.y * v.y + u.z * v.z;
}
Vec3 cross_(Vec3 u, Vec3 v)
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
    return div_vec3(v, f);
}
Vec3 random_vector(float min, float max)
{
    return (Vec3){random_float(min, max), random_float(min, max), random_float(min, max)};
}
Vec3 random_in_unit_sphere()
{
    while (1)
    {
        Vec3 v = random_vector(-1, 1);
        if (length_squared(v) <= 1)
            return v;
    }
}
Vec3 random_unit_vector()
{
    Vec3 u = random_in_unit_sphere();
    Vec3 v = unit_vector(u);
    return v;
}
Vec3 point_at(Ray ray, float t)
{
    return (Vec3){ray.org.x + t * ray.dir.x, ray.org.y + t * ray.dir.y, ray.org.z + t * ray.dir.z};
}
time_t get_time()
{
    struct timespec time_;
    clock_gettime(CLOCK_MONOTONIC, &time_);
    return (time_.tv_sec * 1000 + time_.tv_nsec / 1000000);
}
Obj new_sphere(Vec3 center, float radius, Color color, Mat mat)
{
    Obj new = {0};
    new.type = sphere_;
    new.center = center;
    new.radius = radius;
    new.color = color;
    new.mat = mat;
    return new;
}
Obj new_plan(Vec3 normal, float d, Color color, Mat mat)
{
    Obj new = {0};
    new.type = plan_;
    new.normal = unit_vector(normal);
    new.d = d;
    new.color = color;
    new.mat = mat;
    return new;
}
Obj new_triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    Obj new = {0};
    new.type = triangle_;
    new.p1 = p1;
    new.p2 = p2;
    new.p3 = p3;
    new.normal = cross_(unit_vector(sub_vec3(p2, p1)), unit_vector(sub_vec3(p3, p1)));
    new.color = color;
    new.mat = mat;
    return new;
}