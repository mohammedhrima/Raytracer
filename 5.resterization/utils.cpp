#include "../0.headers/SDL.h"
#include <iostream>
#include <unistd.h>
#include <climits>

// key eventt
#define RELEASE SDL_KEYUP
#define PRESS SDL_KEYDOWN
#define QUIT SDL_QUIT
#define MOUSE_MOTION SDL_MOUSEMOTION
#define MOUSE_DOWN SDL_MOUSEBUTTONDOWN
#define MOUSE_UP SDL_MOUSEBUTTONUP
#define MOUSE_SCROLL SDL_MOUSEWHEEL

// dimentions
#define WIDTH 500
#define HEIGHT WIDTH

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

// can't do mutiple frames with no threads
#define THREADS_LEN 9
#define FRAMES_LEN 0

#define ZERO .0001f
#define PI 3.1415926535897932385

#ifndef FOCAL_LEN
#define FOCAL_LEN 10
#endif

// R  G  B
// 24 16 8
#define COLOR(r, g, b) r << 24 | g << 16 | b << 8

#define COLORS                  \
    (Color[])                   \
    {                           \
        {0.30, 0.92, 0.64},     \
            {0.39, 0.92, 0.63}, \
            {0.42, 0.92, 0.80}, \
            {0.47, 0.16, 0.92}, \
            {0.42, 0.58, 0.92}, \
            {0.92, 0.19, 0.15}, \
            {0.42, 0.92, 0.72}, \
            {0.42, 0.87, 0.92}, \
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
    rectangle_,
    cylinder_,
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

#define BACKGROUND(a) \
    (Color) { (1.0f - a) + a * .5f, (1.0f - a) + a * .7f, 1.0f }

typedef struct
{
    Type type;
    Mat mat;
    Color color;
    Vec3 normal;
    Color light_color;
    float light_intensity;
    union
    {
        // Sphere
        struct
        {
            Vec3 center;
            float radius;
            float speed;
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
    Vec3 camera;
    Vec3 cam_dir;
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
    uint32_t *pixels;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *screen_texture;
    SDL_Event ev;
    Scene scene;
} Win;

Color color(float r, float g, float b)
{
    return (Color){r / 255.999f, g / 255.999f, b / 255.999f};
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
Vec3 operator*(float t, Vec3 v)
{
    return (Vec3){t * v.x, t * v.y, t * v.z};
}
Vec3 operator*(Vec3 v, float t)
{
    return (Vec3){t * v.x, t * v.y, t * v.z};
}
Vec3 operator*(Vec3 l, Vec3 r)
{
    return (Vec3){l.x * r.x, l.y * r.y, l.z * r.z};
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
Vec3 operator/(Vec3 l, Vec3 r)
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
float unit_dot(Vec3 u, Vec3 v)
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

// SDL functions
Win *new_window(int width, int height, char *title)
{
    Win *win;
    win = (Win *)calloc(1, sizeof(Win));
    win->width = width;
    win->height = height;
    win->pixels = (uint32_t *)calloc(width * height, sizeof(uint32_t));
    win->scene.camera = (Vec3){0, 0, 0};
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Error opening window" << std::endl;
        SDL_Quit();
        exit(-1);
    }
    win->window = SDL_CreateWindow(title, 1000, 100, width, height, SDL_WINDOW_SHOWN);
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
    SDL_DestroyTexture(win->screen_texture);
    SDL_DestroyRenderer(win->renderer);
    SDL_DestroyWindow(win->window);
    SDL_Quit();
}

int is_mouse_down;
int on_mouse_move(Win *win, int x, int y)
{
    return 0;
}

void listen_on_events(Win *win)
{
    while (SDL_PollEvent(&win->ev) != 0)
    {
        switch (win->ev.type)
        {
        case QUIT:
            close_window(win);
            exit(0);
            break;
        case MOUSE_DOWN:
            is_mouse_down = 1;
            break;
        case MOUSE_UP:
            is_mouse_down = 0;
            break;
        case MOUSE_MOTION:
            break;
        case MOUSE_SCROLL:
            break;
        case PRESS:
            switch (win->ev.key.keysym.sym)
            {
            case ESC:
                close_window(win);
                exit(0);
                break;
            case FORWARD:
            case BACKWARD:
            case UP:
            case DOWN:
            case LEFT:
            case RIGHT:
            case RESET:
                break;
            default:
                std::cout << "key cliqued: " << win->ev.key.keysym.sym << std::endl;
                break;
            }
            break;
        default:
            break;
        }
    }
}