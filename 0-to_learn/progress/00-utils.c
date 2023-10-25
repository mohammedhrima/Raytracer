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
#include <string.h>

#define FRAMES_LEN 500

// key board
#define ESC 53
#define UP 126
#define DOWN 125
#define LEFT 123
#define RIGHT 124
#define FORWARD 91
#define BACKWARD 84

#ifndef FOCAL_LEN
#define FOCAL_LEN 2
#endif

// reset
#define RESET 15
// recoding mode
#define C_KEY 8
// play mode
#define P_KEY 35
// save buffer
#define S_KEY 0

// start / stop recording
// play / pause frames
#define SPACE 49

// move between buffers
#define PLUS 69
#define MINUS 78

// delete frames buffer
#define DELETE 117

#define ROTATE_LEFT 88
#define ROTATE_RIGHT 86

// mouse
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define SCROLL_UP 5
#define SCROLL_DOWN 4

#define ZERO .0001f
#define RED 0xff0000

#define COLORS                  \
    {                           \
        {0.92, 0.19, 0.15},     \
            {0.42, 0.92, 0.72}, \
            {0.42, 0.87, 0.92}, \
            {0.30, 0.92, 0.64}, \
            {0.39, 0.92, 0.63}, \
            {0.42, 0.92, 0.80}, \
            {0.47, 0.16, 0.92}, \
            {0.42, 0.58, 0.92}, \
            {0.92, 0.40, 0.30}, \
            {0.61, 0.75, 0.24}, \
            {0.83, 0.30, 0.92}, \
            {0.23, 0.92, 0.08}, \
    }

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
    triangle_,
    hemisphere_
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
            // hemi Sphere
            Vec3 heminormal;
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
    Vec3 cam_dir;
    Vec3 screen_u;
    Vec3 screen_v;
    Vec3 pixel_u;
    Vec3 pixel_v;
    Vec3 first_pixel;
    Vec3 u, v, w;
    Obj *objects;
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
    void *title;

    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    Scene scene;
} Win;

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
        printf("Error 1: dividing by 0\n");
        exit(1);
    }
    return mul_vec3(1 / t, v);
}
Vec3 div_vec3_(Vec3 l, Vec3 r)
{
    if (r.x == 0.0 || r.y == 0.0 || r.z == 0.0)
    {
        printf("Error 2: dividing by 0\n");
        exit(1);
    }
    return (Vec3){l.x / r.x, l.y / r.y, l.z / r.z};
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
Obj new_hemisphere(Vec3 center, float radius, Vec3 normal, Color color, Mat mat)
{
    Obj new = {0};
    new.type = hemisphere_;
    new.center = center;
    new.radius = radius;
    new.color = color;
    new.mat = mat;
    new.heminormal = normal; // add_vec3(normal, center);
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

typedef struct
{
    int v, vt, vn;
} FaceVertex;

void parse_obj(Scene *scene, char *name)
{
    FILE *file = fopen(name, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open the .obj file.\n");
        exit(1);
    }

    char line[128];
    Vec3 *vertices = NULL;
    Vec3 *normals = NULL;
    Vec3 *textures = NULL;
    Obj *triangles = scene->objects;
    int numVertices = 0;
    int numNormals = 0;
    int numTextures = 0;
    // scene->pos = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            Vec3 vertex;
            if (sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z) == 3)
            {

                // vertex.x *= 2;
                // vertex.y *= 2;
                vertex.z *= 2;

                vertices = (Vec3 *)realloc(vertices, (numVertices + 1) * sizeof(Vec3));
                vertices[numVertices++] = vertex;
            }
        }
        // else if (line[0] == 'v' && line[1] == 'n')
        // {
        //     Vec3 normal;
        //     if (sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z) == 3)
        //     {
        //         numNormals++;
        //         normals = (Vec3 *)realloc(normals, numNormals * sizeof(Vec3));
        //         normals[numNormals - 1] = normal;
        //     }
        // }
        // else if (line[0] == 'v' && line[1] == 't')
        // {
        //     Vec3 texture;
        //     if (sscanf(line, "vt %f %f %f", &texture.x, &texture.y, &texture.z) >= 2)
        //     {
        //         numTextures++;
        //         textures = (Vec3 *)realloc(textures, numTextures * sizeof(Vec3));
        //         textures[numTextures - 1] = texture;
        //     }
        // }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            FaceVertex face[3];
            if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &face[0].v, &face[0].vt, &face[0].vn,
                       &face[1].v, &face[1].vt, &face[1].vn,
                       &face[2].v, &face[2].vt, &face[2].vn) == 9)
            {
                Vec3 p1 = vertices[face[0].v - 1];
                Vec3 p2 = vertices[face[1].v - 1];
                Vec3 p3 = vertices[face[2].v - 1];
                triangles[scene->pos] = new_triangle(p1, p2, p3, (Color){1, 0, 0}, Abs_);
                scene->pos++;
            }

            // printf("new triangle: \n");
            // printf("\tp1(%f, %f, %f)\n", triangles[scene->pos - 1].p1.x, triangles[scene->pos - 1].p1.y, triangles[scene->pos - 1].p1.z);
            // printf("\tp2(%f, %f, %f)\n", triangles[scene->pos - 1].p2.x, triangles[scene->pos - 1].p2.y, triangles[scene->pos - 1].p2.z);
            // printf("\tp3(%f, %f, %f)\n", triangles[scene->pos - 1].p3.x, triangles[scene->pos - 1].p3.y, triangles[scene->pos - 1].p3.z);
        }
    }
    fclose(file);

    if (numVertices == 0 || scene->pos == 0)
    {
        fprintf(stderr, "Error: No vertices or triangles found in the .obj file.\n");
        exit(1);
    }
}

// int listen_on_mouse(int code, Win *win)
// {
//     /*
//         events:
//             clique: left, right
//             scroll: up down
//     */
//     Scene *scene = &win->scene;
//     struct
//     {
//         Vec3 move;
//         char *msg;
//     } trans[1000] = {
//         [SCROLL_UP] = {(Vec3){0, 0, -.1}, "forward"},
//         [SCROLL_DOWN] = {(Vec3){0, 0, .1}, "backward"},
//     };
//     switch (code)
//     {
//     case SCROLL_UP:
//     case SCROLL_DOWN:
//         printf("%s\n", trans[code].msg);
//         translate(scene, trans[code].move);
//         break;
//     default:
//         printf("mouse: %d\n", code);
//         return 0;
//     }
//     init(win);
//     // printf("Pos (%f, %f, %f)\n", scene->camera.x, scene->camera.y, scene->camera.z);
//     return 0;
// }