#include "../0.headers/SDL.h"
#include <iostream>
#include <unistd.h>

// key eventt
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

// can't do mutiple frames with no threads
#define THREADS_LEN 4
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
            {0.30, 0.92, 0.64}, \
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
    (Color) { (1.0 - a) + a * .5, (1.0 - a) + a * .7, 1.0 }

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
    Vec3 dir;
    Vec3 org;
} t_ray;

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
#if THREADS_LEN
    _Atomic int thread_finished[THREADS_LEN];
#endif
} Win;

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
float degrees_to_radians(float degrees)
{
    return degrees * PI / 180.0;
}
Color color(float r, float g, float b)
{
    return (Color){r / 255.999, g / 255.999, b / 255.999};
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
Vec3 point_at(t_ray ray, float t)
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
    Obj obj = {};
    obj.type = sphere_;
    obj.center = center;
    obj.radius = radius;
    obj.color = color;
    obj.mat = mat;
    return obj;
}

Obj new_plan(Vec3 normal, float d, Color color, Mat mat)
{
    Obj obj = {};
    obj.type = plan_;
    obj.normal = unit_vector(normal);
    obj.d = d;
    obj.color = color;
    obj.mat = mat;
    return obj;
}
Obj new_triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    Obj obj = {};
    obj.type = triangle_;
    obj.p1 = p1;
    obj.p2 = p2;
    obj.p3 = p3;

    obj.normal = cross(unit_vector(p2 - p1), unit_vector(p3 - p1));
    obj.color = color;
    obj.mat = mat;

    return obj;
}

Obj new_rectangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    Obj obj = {};
    obj.type = rectangle_;
    obj.p1 = p1;
    obj.p2 = p2;
    obj.p3 = p2;

    obj.normal = cross(unit_vector(p2 - p1), unit_vector(p3 - p1));
    obj.color = color;
    obj.mat = mat;

    return obj;
}

Obj new_cylinder(Vec3 center, float radius, Vec3 normal, Color color, Mat mat)
{
    Obj obj = {};
    obj.type = cylinder_;
    obj.center = center;
    obj.radius = radius;
    obj.normal = normal;
    obj.color = color;
    obj.mat = mat;
    return obj;
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
void init(Win *win);
void update_window(Win *win)
{
    // Update the texture with the modified pixel data
    SDL_UpdateTexture(win->screen_texture, NULL, win->pixels, win->width * 4);
    // Copy the texture to the renderer
    SDL_RenderCopy(win->renderer, win->screen_texture, NULL, NULL);
    // Present the renderer
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

// Raytracing
// hit functions
float hit_sphere(Obj sphere, t_ray ray, float min, float max)
{
    Vec3 OC = ray.org - sphere.center;

    float a = length_squared(ray.dir);
    float half_b = unit_dot(OC, ray.dir);
    float c = length_squared(OC) - sphere.radius * sphere.radius;
    float delta = half_b * half_b - a * c;
    if (delta < .0)
        return -1.0;
    float sq_delta = sqrtf(delta);
    float sol = (-half_b - sq_delta) / a;
    if (sol <= min || sol >= max)
        sol = (-half_b + sq_delta) / a;
    if (sol <= min || sol >= max)
        return -1.0;
    return (sol);
}

float hit_cylinder(Obj cylin, t_ray ray, float min, float max)
{
    float t = hit_sphere(cylin, ray, min, max);
    if (t != -1.0 && unit_dot(unit_vector(point_at(ray, t)), cylin.normal) == 0)
        return t;
    return -1.0;
}

float hit_plan(Obj plan, t_ray ray, float min, float max)
{
    float t = plan.d - unit_dot(plan.normal, ray.org);
    float div = unit_dot(ray.dir, plan.normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;
    return (t);
}

float hit_triangle(Obj trian, t_ray ray, float min, float max)
{
    float t = unit_dot(trian.normal, (trian.p1 - ray.org));
    float div = unit_dot(trian.normal, ray.dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;
#if 0
    Vec3 p = point_at(ray, t) - trian.p1;
    Vec3 u = trian.p2 - trian.p1;
    Vec3 v = trian.p3 - trian.p1;
    Vec3 normal = cross(u, v);
    Vec3 w = normal / unit_dot(normal, normal);
    float alpha = unit_dot(cross(p, v), w);
    float beta = -unit_dot(cross(p, u), w);
#endif
    Vec3 u = trian.p2 - trian.p1;
    Vec3 v = trian.p3 - trian.p1;
    Vec3 rp = point_at(ray, t) - trian.p1;
    float d = u.x * v.y - u.y * v.x;
    if (d == 0)
        return -1.0;
    float alpha = (rp.x * v.y - rp.y * v.x) / d;
    float beta = (u.x * rp.y - u.y * rp.x) / d;

    if (alpha < 0 || beta < 0 || alpha + beta > 1)
        return -1;

    return t;
}

float hit_rectangle(Obj rec, t_ray ray, float min, float max)
{
    float t = unit_dot(rec.normal, (rec.p1 - ray.org));
    float div = unit_dot(rec.normal, ray.dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;

    Vec3 u = rec.p2 - rec.p1;
    Vec3 v = rec.p3 - rec.p1;
    Vec3 rp = point_at(ray, t) - rec.p1;
    float d = u.x * v.y - u.y * v.x;
    if (d == 0)
        return -1.0;
    float alpha = (rp.x * v.y - rp.y * v.x) / d;
    float beta = (u.x * rp.y - u.y * rp.x) / d;

    if (alpha < 0 || beta < 0 || alpha > 1 || beta > 1)
        return -1;

    return t;
}
// rendering
t_ray render_object(Obj obj, t_ray ray, float closest)
{
    // point coordinates
    Vec3 cp_norm;
    Vec3 p = point_at(ray, closest);

    switch (obj.type)
    {
    case sphere_:
        cp_norm = unit_vector(p - obj.center);
        break;
    case plan_:
    case triangle_:
    case rectangle_:
    case cylinder_:
        cp_norm = obj.normal;
        break;
    default:
        std::cerr << "Unkown object type" << std::endl;
        exit(1);
        break;
    }

    bool same_dir = unit_dot(cp_norm, ray.dir) >= 0;
    if (same_dir) // to be used when drawing triangle
        cp_norm = (Vec3){-cp_norm.x, -cp_norm.y, -cp_norm.z};
    Vec3 ranv = random_unit_vector();
    Vec3 ndir;
    if (obj.mat == Refl_)
    {
        float val;
        val = -2 * unit_dot(ray.dir, cp_norm);
        ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
    }
    else if (obj.mat == Refr_)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);

        float cos_theta = unit_dot(ray.dir, cp_norm) / (length(ray.dir) * length(cp_norm));
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (refraction_ratio * sin_theta > 1.0)
        {
            // Reflect
            float val;
            val = -2 * unit_dot(ray.dir, cp_norm);
            ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
        }
        else
        {
            // Refract
            Vec3 ray_dir = unit_vector(ray.dir);
            Vec3 Perp = refraction_ratio * (ray_dir - (unit_dot(ray_dir, cp_norm) * cp_norm));
            Vec3 Para = sqrt(1 - pow(length(Perp), 2)) * (-1 * cp_norm);
            ndir = Perp + Para;
        }
    }
    else if (obj.mat == Abs_)
        ndir = cp_norm + ranv;
    return (t_ray){.org = p, .dir = ndir};
}

#if 1
Color ray_color(Win *win, t_ray ray, int depth)
{
    Scene *scene = &win->scene;
    Color light = {};
    Color attenuation = {1, 1, 1};

    for (int bounce = 0; bounce < depth; bounce++)
    {
        float closest = FLT_MAX;
        int hit_index = -1;
        float x = 0;
        for (int i = 0; i < scene->pos; i++)
        {
            if (scene->objects[i].type == sphere_)
                x = hit_sphere(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == cylinder_)
                x = hit_cylinder(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == plan_)
                x = hit_plan(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == triangle_)
                x = hit_triangle(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == rectangle_)
                x = hit_rectangle(scene->objects[i], ray, ZERO, closest);
            if (x > .0)
            {
                hit_index = i;
                closest = x;
            }
        }
        if (hit_index != -1)
        {
            Obj *obj = &scene->objects[hit_index];
            ray = render_object(*obj, ray, closest);
            light = light + attenuation * obj->light_intensity * obj->light_color;
            attenuation = attenuation * obj->color;
        }
        else
        {
            light = light + attenuation * BACKGROUND(0.5 * (unit_vector(ray.dir).y + 1.0));
            break;
        }
        if (attenuation.x <= ZERO && attenuation.y <= ZERO && attenuation.z <= ZERO)
            break;
    }
    return light;
}
#else
Color ray_color_recursive(Scene *scene, t_ray ray, int depth, Color light, Color attenuation)
{
    // TODO: must be understood
    if (depth == 0)
        return attenuation * BACKGROUND(0.5 * (unit_vector(ray.dir).y + 1.0));
    if (attenuation.x <= ZERO && attenuation.y <= ZERO && attenuation.z <= ZERO)
        return light;

    float closest = FLT_MAX;
    int hit_index = -1;
    float x = 0;
    for (int i = 0; i < scene->pos; i++)
    {
        if (scene->objects[i].type == sphere_)
            x = hit_sphere(scene->objects[i], ray, ZERO, closest);
        else if (scene->objects[i].type == cylinder_)
            x = hit_cylinder(scene->objects[i], ray, ZERO, closest);
        else if (scene->objects[i].type == plan_)
            x = hit_plan(scene->objects[i], ray, ZERO, closest);
        else if (scene->objects[i].type == triangle_)
            x = hit_triangle(scene->objects[i], ray, ZERO, closest);
        else if (scene->objects[i].type == rectangle_)
            x = hit_rectangle(scene->objects[i], ray, ZERO, closest);
        if (x > .0)
        {
            hit_index = i;
            closest = x;
        }
    }
    if (hit_index != -1)
    {
        Obj *obj = &scene->objects[hit_index];
        ray = render_object(*obj, ray, closest);
        light = light + attenuation * obj->light_intensity * obj->light_color;
        attenuation = attenuation * obj->color;
    }
    else
        light = light + attenuation * BACKGROUND(0.5 * (unit_vector(ray.dir).y + 1.0));
    return ray_color_recursive(scene, ray, depth - 1, light, attenuation);
}
Color ray_color(Win *win, t_ray ray, int depth)
{
    Scene *scene = &win->scene;
    Color light = {};
    Color attenuation = {1, 1, 1};
    return ray_color_recursive(scene, ray, depth, light, attenuation);
}
#endif

// Scene
float y_rotation = 0;
float x_rotation = 0;
Vec3 rotate(Win *win, Vec3 u, int axes, float angle);
void init(Win *win)
{
#if 0
    Scene *scene = &win->scene;

    scene->cam_dir = (Vec3){0, 0.65, -1};
    scene->cam_dir = rotate(win, scene->cam_dir, 'y', x_rotation);
    scene->cam_dir = rotate(win, scene->cam_dir, 'x', y_rotation);
    scene->w = -1 * unit_vector(scene->cam_dir); // step in z axis and z
    Vec3 upv = (Vec3){0, 1, 0};

     upv = rotate(win, upv, 'y', x_rotation);
    upv = rotate(win,  upv, 'x', y_rotation);

    float tang = tan(degrees_to_radians(20) / 2);
    float screen_height = 2 * tang * FOCAL_LEN;
    float screen_width = screen_height * ((float)win->width / win->height);

    scene->u = unit_vector(cross(upv, scene->w)); // x+ (get v vector)
    scene->v = unit_vector(cross(scene->w, scene->u));   // y+ (get u vector)

    // viewport steps
    Vec3 screen_u = screen_width * scene->u;
    Vec3 screen_v = -screen_height * scene->v;
    // window steps
    scene->pixel_u = screen_u / win->width;
    scene->pixel_v = screen_v / win->height;

    Vec3 screen_center = scene->camera + (-FOCAL_LEN* scene->w);
    Vec3 upper_left = screen_center - (screen_u + screen_v) / 2;
    scene->first_pixel = upper_left + (scene->pixel_u + scene->pixel_v) / 2;
#else
    Scene *scene = &win->scene;

    // scene
    scene->cam_dir = (Vec3){0, 0, -FOCAL_LEN};
    scene->cam_dir = rotate(win, scene->cam_dir, 'y', x_rotation);
    scene->cam_dir = rotate(win, scene->cam_dir, 'x', y_rotation);

    scene->w = unit_vector((Vec3){0, 0, 0} - scene->cam_dir);
    Vec3 upv = rotate(win, (Vec3){0, 1, 0}, 'y', x_rotation);
    upv = rotate(win, upv, 'x', y_rotation);
    float screen_height = 2.0 * tan(degrees_to_radians(40 / 2)) * FOCAL_LEN;
    float screen_width = screen_height * ((float)win->width / win->height);
    scene->u = unit_vector(cross(upv, scene->w));
    scene->v = unit_vector(cross(scene->w, scene->u));
    scene->pixel_u = (screen_width / win->width) * scene->u;
    scene->pixel_v = -(screen_height / win->height) * scene->v;
    scene->first_pixel = scene->camera - (FOCAL_LEN * scene->w) - (screen_width * scene->u - screen_height * scene->v) / 2 + (scene->pixel_u + scene->pixel_v) / 2;
#endif
}
extern Color *sum;
// int frame;
int old_x;
int old_y;
int old_z;

extern int is_mouse_down;

void translate(Win *win, Vec3 move)
{
    Vec3 x = move.x * win->scene.u;
    Vec3 y = move.y * win->scene.v;
    Vec3 z = move.z * win->scene.w;

    win->scene.camera = win->scene.camera + x + y + z;
}
static int angle1;
Vec3 rotate(Win *win, Vec3 u, int axes, float angle)
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

int on_mouse_move(Win *win, int x, int y)
{
    Scene *scene = &win->scene;
    if (is_mouse_down)
    {
        int dx = x - old_x;
        int dy = y - old_y;
        float tanx = atan((float)dx / win->width);
        float tany = atan((float)dy / win->height);
        if (win->scene.cam_dir.z > win->scene.camera.z)
            tany = -tany;
        x_rotation -= tanx;
        y_rotation -= tany;
    }
    old_x = x;
    old_y = y;
    return 0;
}

typedef struct
{
    pthread_t thread;
    Win *win;
    int idx;
} Multi;
Multi *new_multi(int idx, Win *win)
{
    Multi *multi = (Multi *)calloc(1, sizeof(Multi));
    multi->idx = idx;
    multi->win = win;
    return multi;
}
void divideWindow(Win *win, int threadNum, int &x_start, int &y_start, int &w, int &h)
{
    int cols = ceil(sqrt(THREADS_LEN));
    int rows = ceil((float)THREADS_LEN / cols);
    int col = threadNum % cols;
    int row = threadNum / cols;
    int cellWidth = win->width / cols;
    int cellHeight = win->height / rows;
    x_start = col * cellWidth;
    y_start = row * cellHeight;
    w = cellWidth - 1;
    h = cellHeight - 1;
}

Color *sum;
_Atomic(int) frame_index;
int is_mouse_down = 0;
int frame_shots;

#if THREADS_LEN
void *Multi_TraceRay(void *arg)
{
    Multi *multi = (Multi *)arg;
    Win *win = multi->win;
    Scene *scene = &win->scene;

    int x_start, y_start, width, height;
    divideWindow(win, multi->idx, x_start, y_start, width, height);
    while (1)
    {
        if (FRAMES_LEN && frame_shots == FRAMES_LEN)
            break;
        if (win->thread_finished[multi->idx])
        {
            usleep(1000);
            continue;
        }
        for (int h = y_start; h < y_start + height; h++)
        {
            for (int w = x_start; w < x_start + width; w++)
            {
                int rays_per_pixel = 9;
                Color pixel = (Color){0, 0, 0};
#if FRAMES_LEN
                for (int i = 0; i < rays_per_pixel; i++)
                {
#endif
                    Vec3 pixel_center = scene->first_pixel + ((float)w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
                    Vec3 dir = pixel_center - scene->camera;
                    t_ray ray = (t_ray){.org = scene->camera, .dir = dir};
                    pixel = pixel + ray_color(win, ray, 5);
#if FRAMES_LEN
                }
                pixel = pixel / rays_per_pixel;
#endif
                sum[h * win->width + w] = sum[h * win->width + w] + pixel;
                pixel = sum[h * win->width + w] / (float)frame_index;
                if (pixel.r > 1)
                    pixel.r = 1;
                if (pixel.g > 1)
                    pixel.g = 1;
                if (pixel.b > 1)
                    pixel.b = 1;
                win->pixels[h * win->width + w] = COLOR((int)(255.999 * sqrtf(pixel.r)), (int)(255.999 * sqrtf(pixel.g)), (int)(255.999 * sqrtf(pixel.b)));
            }
        }
        win->thread_finished[multi->idx] = 1;
    }
    std::cout << "thread " << multi->idx << "finished" << std::endl;
    return NULL;
}
#else

void TraceRay(Win *win)
{
    std::cout << "Tracing ray" << std::endl;
    Scene *scene = &win->scene;
    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            int rays_per_pixel = 256;
            Color pixel = (Color){0, 0, 0};
            for (int i = 0; i < rays_per_pixel; i++)
            {
                Vec3 pixel_center = scene->first_pixel + ((float)w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
                Vec3 dir = pixel_center - scene->camera;
                t_ray ray = (t_ray){.org = scene->camera, .dir = dir};
                pixel = pixel + ray_color(win, ray, 25);
            }
            pixel = pixel / rays_per_pixel;
            if (pixel.r > 1)
                pixel.r = 1;
            if (pixel.g > 1)
                pixel.g = 1;
            if (pixel.b > 1)
                pixel.b = 1;
            win->pixels[h * win->width + w] = COLOR((int)(255.999 * sqrtf(pixel.r)), (int)(255.999 * sqrtf(pixel.g)), (int)(255.999 * sqrtf(pixel.b)));
        }
        std::cout << "remaining: " << win->height - h << std::endl;
    }
    std::cout << "render" << std::endl;
}
#endif

void listen_on_events(Win *win, int &quit)
{
    struct
    {
        Vec3 move;
        char *msg;
    } trans[1000] = {
        [FORWARD - 1073741900] = {(Vec3){0, 0, -1}, (char *)"forward"},
        [BACKWARD - 1073741900] = {(Vec3){0, 0, 1}, (char *)"backward"},
        [UP - 1073741900] = {(Vec3){0, 1, 0}, (char *)"up"},
        [DOWN - 1073741900] = {(Vec3){0, -1, 0}, (char *)"down"},
        [LEFT - 1073741900] = {(Vec3){-1, 0, 0}, (char *)"left"},
        [RIGHT - 1073741900] = {(Vec3){1, 0, 0}, (char *)"right"},
    };
    Scene *scene = &win->scene;
    while (SDL_PollEvent(&win->ev) != 0)
    {
        switch (win->ev.type)
        {
        case QUIT:
            quit = true;
            break;
#if THREADS_LEN
        case MOUSE_DOWN:
            if (win->ev.button.button == MOUSE_LEFT)
                std::cout << "Left mouse button clicked at (" << win->ev.button.x << ", " << win->ev.button.y << ")" << std::endl;
            is_mouse_down = 1;
            break;
        case MOUSE_UP:
            is_mouse_down = 0;
            break;
        case MOUSE_MOTION:
            on_mouse_move(win, win->ev.motion.x, win->ev.motion.y);
            if (is_mouse_down)
            {
                // scene->upv = (Vec3){0, 1, 0};
                frame_index = 1;
                memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
                init(win);
            }
            break;
        case MOUSE_SCROLL:
            if (win->ev.wheel.y > 0)
                translate(win, 2 * trans[FORWARD - 1073741900].move);
            else
                translate(win, 2 * trans[BACKWARD - 1073741900].move);
            frame_index = 1;
            memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
            init(win);
            break;
#endif
        case PRESS:
            // case RELEASE:
            switch (win->ev.key.keysym.sym)
            {
            case ESC:
                quit = true;
                break;
#if THREADS_LEN
            case FORWARD:
            case BACKWARD:
            case UP:
            case DOWN:
            case LEFT:
            case RIGHT:
                translate(win, trans[win->ev.key.keysym.sym - 1073741900].move);
                frame_index = 1;
                memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
                init(win);
                break;
            case RESET:
                frame_index = 1;
                memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
                x_rotation = 0;
                y_rotation = 0;
                scene->camera = (Vec3){0, 0, FOCAL_LEN};
                init(win);
                std::cout << "Reset" << std::endl;
                break;
#endif
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