#ifndef HEADER
#define HEADER
#include "./SDL/SDL.h"
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

#define THREADS_LEN 4
#define FRAMES_LEN 0
#define ZERO .0001f
#define PI 3.1415926535897932385
#define FOCAL_LEN 1

// R  G  B
// 24 16 8
#define SDL_COLOR(r, g, b) r << 24 | g << 16 | b << 8
#define COLOR(r, g, b) (Color) { r / 255.999, g / 255.999, b / 255.999 }
#define FOV 45

// structs
typedef enum
{
    REFLECT = 11,
    REFRACT,
    ABSORABLE
} Mat;

typedef enum
{
    SPHERE = 22,
    PLAN,
    TRIANGLE,
    RECTANGLE,
    CYLINDER,
    CONE,
    LIGHT,
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

// Global
extern _Atomic int frame_index;
// prototypes
Vec3 calc(Vec3 l, char c, Vec3 r);
Vec3 scale(Vec3 v, char c, float t);
float degrees_to_radians(float degrees);
float pow2(float x);
Vec3 operator+(Vec3 l, Vec3 r);
Vec3 operator-(Vec3 l, Vec3 r);
Vec3 operator*(Vec3 l, Vec3 r);
Vec3 operator*(float t, Vec3 v);
Vec3 operator*(Vec3 v, float t);
Vec3 operator/(Vec3 v, float t);
std::ostream &operator<<(std::ostream &out, Vec3 &v);
float length_squared(Vec3 v);
float length(Vec3 v);
float dot(Vec3 u, Vec3 v);
Vec3 cross(Vec3 u, Vec3 v);
Vec3 unit_vector(Vec3 v);
Vec3 random_unit_vector();
Vec3 point_at(Ray *ray, float t);
time_t get_time();
float random_float(float min, float max);
Obj *new_sphere(Vec3 center, float radius, Color color, Mat mat);
Obj *new_plan(Vec3 normal, float d, Color color, Mat mat);
Obj *new_triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat);
Obj *new_rectangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat);
Obj *new_cylinder(Vec3 center, float radius, float height, Vec3 normal, Color color, Mat mat);
Obj *new_cone(Vec3 center, float radius, float height, Vec3 normal, Color color, Mat mat);
Multi *new_multi(Win *win, int idx, int cols, int rows);
void translate(Win *win, Vec3 move);
Vec3 rotate(Vec3 u, int axes, float angle);
Win *new_window(int width, int height, char *title);
void update_window(Win *win);
void close_window(Win *win);
void init(Win *win);
Vec3 rotate(Vec3 u, int axes, float angle);
void add_objects(Win *win);
void listen_on_events(Win *win, int &quit);

#if THREADS_LEN
void *TraceRay(void *arg);
#else
void TraceRay(Win *win);
#endif
#endif