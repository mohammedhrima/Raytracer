#include "../0.headers/SDL.h"
#include <iostream>
#include <unistd.h>
#include <climits>

// key events
#define RELEASE SDL_KEYUP
#define PRESS SDL_KEYDOWN
#define QUIT SDL_QUIT
#define MOUSE_MOTION SDL_MOUSEMOTION
#define MOUSE_DOWN SDL_MOUSEBUTTONDOWN
#define MOUSE_UP SDL_MOUSEBUTTONUP
#define MOUSE_SCROLL SDL_MOUSEWHEEL

// keys
#define ESC 27
#define SPACE 32
#define RESET 114
#define UP 1073741906
#define DOWN 1073741905
#define LEFT 1073741904
#define RIGHT 1073741903
#define FORWARD 1073741920
#define BACKWARD 1073741914
#define MOUSE_LEFT SDL_BUTTON_LEFT

#define NUM_0 1073741922
#define NUM_1 1073741913
#define NUM_2 1073741914
#define NUM_3 1073741915
#define NUM_4 1073741916
#define NUM_5 1073741917
#define NUM_6 1073741918
#define NUM_7 1073741919
#define NUM_8 1073741920
#define NUM_9 1073741921

#define KEY_S 115
#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define KEY_3 51
#define KEY_4 52
#define KEY_5 53
#define KEY_6 54
#define KEY_7 55
#define KEY_8 56
#define KEY_9 57

#define FOV 45
#define THREADS_LEN 0
#define FRAMES_LEN 0
#define ZERO .0001f
#define PI 3.1415926535897932385
#define FOCAL_LEN 1

// R  G  B
// 24 16 8
#define SDL_COLOR(r, g, b) r << 24 | g << 16 | b << 8
#define COLOR(r, g, b) \
    (Color) { r / 255.999, g / 255.999, b / 255.999 }

// structs
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
    rectangle_,
    cylinder_,
    cone_,
    light_,
} Type;

typedef struct
{
    float x;
    float y;
    float z;
} Vec3;
typedef Vec3 Color;

#define BACKGROUND(a) \
    (Color) { 1.0f - a * 0.8f, 1.0f - a * 0.6f, 1.0f }

#define COLORS                                                                                                  \
    (Color[])                                                                                                   \
    {                                                                                                           \
        {1, 1, 1},                                                                                              \
            {0.42, 0.92, 0.80}, {0.47, 0.16, 0.92}, {0.42, 0.58, 0.92}, {0.92, 0.19, 0.15}, {0.42, 0.92, 0.72}, \
            {0.42, 0.87, 0.92}, {0.92, 0.40, 0.30}, {0.61, 0.75, 0.24}, {0.83, 0.30, 0.92}, {0.23, 0.92, 0.08}, \
            {0.30, 0.92, 0.64}, {0.39, 0.92, 0.63}, {0.42, 0.92, 0.80}, {0.47, 0.16, 0.92}, {0.42, 0.58, 0.92}, \
            {0.92, 0.19, 0.15}, {0.42, 0.92, 0.72}, {0.42, 0.87, 0.92}, {0.92, 0.40, 0.30}, {0.61, 0.75, 0.24}, \
            {0.83, 0.30, 0.92}, {0.23, 0.92, 0.08}, {0.25, 0.73, 0.51}, {0.62, 0.17, 0.95}, {0.45, 0.88, 0.29}, \
            {0.76, 0.43, 0.67}, {0.33, 0.70, 0.12}, {0.91, 0.56, 0.78}, {0.08, 0.99, 0.37}, {0.71, 0.22, 0.64}, \
            {0.49, 0.84, 0.53}, {0.14, 0.67, 0.98}, {0.27, 0.75, 0.41}, {0.30, 0.92, 0.64}, {0.39, 0.92, 0.63}, \
    }

typedef struct
{
    Type type;
    Mat mat;
    Color color;
    Vec3 normal;
    float lightness;
    union
    {
        // Sphere
        // cylinder
        // cone
        struct
        {
            Vec3 center;
            float radius;
            float height;
        };
        struct
        {
            union
            {
                // triangle
                // rectangle
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
    Vec3 org;
    Vec3 dir;
} Ray;

typedef struct
{
    Color *sum;
    Vec3 camera;
    Vec3 cam_dir;
    Vec3 pixel_u;
    Vec3 pixel_v;
    Vec3 first_pixel;
    Vec3 u;
    Vec3 v;
    Vec3 w;
    Vec3 upv;
    Obj **objects;
    int pos;
} Scene;

typedef struct
{
    int width;
    int height;
    uint32_t *pixels;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *screen_texture;
    SDL_Event ev;
    Scene scene;
#if THREADS_LEN
    _Atomic int thread_finished[THREADS_LEN];
#endif
} Win;

typedef struct
{
    int x_start;
    int x_end;
    int y_start;
    int y_end;
    pthread_t thread;
    int idx;
    Win *win;
} Multi;


Win *new_window(int width, int height, char *title)
{
    Win *win;
    win = (Win *)calloc(1, sizeof(Win));
    win->width = width;
    win->height = height;
    win->scene.sum = (Color *)calloc(win->width * win->height, sizeof(Color));
    win->pixels = (uint32_t *)calloc(width * height, sizeof(uint32_t));

    win->scene.camera = (Vec3){0, 1.5, 5};

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Error opening window" << std::endl;
        SDL_Quit();
        exit(-1);
    }
    win->window = SDL_CreateWindow(title, 0, 0, width, height, SDL_WINDOW_SHOWN);
    if (win->window == NULL)
    {
        std::cerr << "Error opening window" << std::endl;
        SDL_Quit();
        exit(-1);
    }
    win->renderer = SDL_CreateRenderer(win->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetWindowMinimumSize(win->window, width, height);
    SDL_RenderSetLogicalSize(win->renderer, width, height);
    SDL_RenderSetIntegerScale(win->renderer, (SDL_bool)1);
    win->screen_texture = SDL_CreateTexture(win->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    return win;
}

void update_window(Win *win)
{
    SDL_UpdateTexture(win->screen_texture, NULL, win->pixels, win->width * 4);
    SDL_RenderCopy(win->renderer, win->screen_texture, NULL, NULL);
    SDL_RenderPresent(win->renderer);
}
void close_window(Win *win)
{
    free(win->pixels);
    for (int i = 0; i < win->scene.pos; i++)
        free(win->scene.objects[i]);
    free(win->scene.objects);
    free(win->scene.sum);
    SDL_DestroyTexture(win->screen_texture);
    SDL_DestroyRenderer(win->renderer);
    SDL_DestroyWindow(win->window);
    SDL_Quit();
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
Obj *new_sphere(Vec3 center, float radius, Color color, Mat mat)
{
    std::cout << "new sphere" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = sphere_;
    obj->center = center;
    obj->radius = radius;
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_plan(Vec3 normal, float d, Color color, Mat mat)
{
    std::cout << "new plan" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = plan_;
    obj->normal = unit_vector(normal);
    obj->d = d;
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    std::cout << "new triangle" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = triangle_;
    obj->p1 = p1;
    obj->p2 = p2;
    obj->p3 = p3;
    obj->normal = cross(p2 - p1, p3 - p1);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_rectangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    std::cout << "new rectangle" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = rectangle_;
    obj->p1 = p1;
    obj->p2 = p2;
    obj->p3 = p3;
    obj->normal = cross(p2 - p1, p3 - p1);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_cylinder(Vec3 center, float radius, float height, Vec3 normal, Color color, Mat mat)
{
    std::cout << "new cylinder" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = cylinder_;
    obj->center = center;
    obj->radius = radius;
    obj->height = height;
    obj->normal = unit_vector(normal);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_cone(Vec3 center, float radius, float height, Vec3 normal, Color color, Mat mat)
{
    std::cout << "new cone" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = cone_;
    obj->center = center;
    obj->radius = radius;
    obj->height = height;
    obj->normal = unit_vector(normal);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Multi *new_multi(Win *win, int idx, int cols, int rows)
{
    Multi *multi = (Multi *)calloc(1, sizeof(Multi));
    int cellWidth = win->width / cols;
    int cellHeight = win->height / rows;
    multi->x_start = (idx % cols) * cellWidth;
    multi->y_start = (idx / cols) * cellHeight;
    multi->x_end = multi->x_start + cellWidth - 1;
    multi->y_end = multi->y_start + cellHeight - 1;
    multi->idx = idx;
    multi->win = win;
    return multi;
}