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

#define THREADS_LEN 4
#define FRAMES_LEN 0
#define ZERO .0001f
#define PI 3.1415926535897932385
#define FOCAL_LEN 10

// R  G  B
// 24 16 8
#define SDL_COLOR(r, g, b) r << 24 | g << 16 | b << 8

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

typedef Vec3 t_vec3;
t_vec3 calc(t_vec3 l, char c, t_vec3 r)
{
    if (c == '+')
        return ((t_vec3){.x = l.x + r.x, .y = l.y + r.y, .z = l.z + r.z});
    if (c == '-')
        return ((t_vec3){.x = l.x - r.x, .y = l.y - r.y, .z = l.z - r.z});
    if (c == '*')
        return ((t_vec3){.x = l.x * r.x, .y = l.y * r.y, .z = l.z * r.z});
    if (c == '/')
    {
        if (r.x == 0.0 || r.y == 0.0 || r.z == 0.0)
        {
            printf("Error 2: dividing by 0\n");
            exit(1);
        }
        return ((t_vec3){.x = l.x / r.x, .y = l.y / r.y, .z = l.z / r.z});
    }

    return ((t_vec3){});
}

t_vec3 scale(t_vec3 v, char c, float t)
{
    if (c == '*')
        return ((t_vec3){.x = t * v.x, .y = t * v.y, .z = t * v.z});
    if (c == '/')
    {
        if (t == 0)
        {
            printf("Error 1: dividing by 0\n");
            exit(1);
        }
        return ((t_vec3){.x = v.x / t, .y = v.y / t, .z = v.z / t});
    }
    return ((t_vec3){});
}

// globals
_Atomic int frame_index;
int is_mouse_down;
int frame_shots;
float y_rotation;
float x_rotation;
int old_x;
int old_y;

// window
Win *new_window(int width, int height, char *title)
{
    Win *win;
    win = (Win *)calloc(1, sizeof(Win));
    win->width = width;
    win->height = height;
    win->scene.sum = (Color *)calloc(win->width * win->height, sizeof(Color));
    win->pixels = (uint32_t *)calloc(width * height, sizeof(uint32_t));

    win->scene.camera = (Vec3){0, 0, FOCAL_LEN};

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
// new object
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
// raytracing
Vec3 rotate(Vec3 u, int axes, float angle);
void init(Win *win)
{
    Scene *scene = &win->scene;
    frame_index = 1;

    scene->cam_dir = rotate((Vec3){0, 0, -FOCAL_LEN}, 'x', y_rotation);
    scene->cam_dir = rotate(scene->cam_dir, 'y', x_rotation);

    scene->w = -1 * unit_vector(scene->cam_dir); // step in z axis and z
    Vec3 upv = rotate((Vec3){0, 1, 0}, 'x', y_rotation);
    upv = rotate(upv, 'y', x_rotation);

    float screen_height = 2 * tan(degrees_to_radians(40 / 2)) * FOCAL_LEN;
    float screen_width = screen_height * ((float)win->width / win->height);

    scene->u = unit_vector(cross(upv, scene->w));      // x+ (get u vector)
    scene->v = unit_vector(cross(scene->w, scene->u)); // y+ (get v vector)

    // viewport steps
    Vec3 screen_u = screen_width * scene->u;
    Vec3 screen_v = -screen_height * scene->v;
    // window steps
    scene->pixel_u = screen_u / win->width;
    scene->pixel_v = screen_v / win->height;

    Vec3 screen_center = scene->camera + (-FOCAL_LEN * scene->w);
    Vec3 upper_left = screen_center - (screen_u + screen_v) / 2;
    scene->first_pixel = upper_left + (scene->pixel_u + scene->pixel_v) / 2;
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

int on_mouse_move(Win *win, int x, int y)
{
    Scene *scene = &win->scene;
    if (is_mouse_down)
    {
        int dx = x - old_x;
        int dy = y - old_y;

        x_rotation -= ((float)dx / (win->width * 0.5f)) * PI * 0.5f;
        y_rotation -= ((float)dy / (win->height * 0.5f)) * PI * 0.5f;
    }
    old_x = x;
    old_y = y;
    return 0;
}

Obj *Global;
void listen_on_events(Win *win, int &quit)
{
    struct
    {
        Vec3 move;
        char *msg;
    } trans[1000];
    trans[FORWARD - 1073741900] = {(Vec3){0, 0, -1}, (char *)"forward"};
    trans[BACKWARD - 1073741900] = {(Vec3){0, 0, 1}, (char *)"backward"};
    trans[UP - 1073741900] = {(Vec3){0, 1, 0}, (char *)"up"};
    trans[DOWN - 1073741900] = {(Vec3){0, -1, 0}, (char *)"down"};
    trans[LEFT - 1073741900] = {(Vec3){-1, 0, 0}, (char *)"left"};
    trans[RIGHT - 1073741900] = {(Vec3){1, 0, 0}, (char *)"right"};

    Vec3 move_obj[100];
    move_obj[NUM_0 - NUM_0] = (Vec3){0, 0, +1};
    move_obj[NUM_0 - NUM_5] = (Vec3){0, 0, -1};
    move_obj[NUM_0 - NUM_8] = (Vec3){0, +1, 0};
    move_obj[NUM_0 - NUM_2] = (Vec3){0, -1, 0};
    move_obj[NUM_0 - NUM_4] = (Vec3){-1, 0, 0};
    move_obj[NUM_0 - NUM_6] = (Vec3){+1, 0, 0};

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
            old_x = win->ev.button.x;
            old_y = win->ev.button.y;
            break;
        case MOUSE_UP:
            is_mouse_down = 0;
            break;
        case MOUSE_MOTION:
            on_mouse_move(win, win->ev.motion.x, win->ev.motion.y);
            if (is_mouse_down)
            {
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                init(win);
            }
            break;
        case MOUSE_SCROLL:
            if (win->ev.wheel.y > 0)
                translate(win, 2 * trans[FORWARD - 1073741900].move);
            else
                translate(win, 2 * trans[BACKWARD - 1073741900].move);
            frame_index = 1;
            memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
            init(win);
            break;
#endif
        case PRESS:
            switch (win->ev.key.keysym.sym)
            {
            case ESC:
                quit = true;
                break;
#if THREADS_LEN
            case UP:
            case DOWN:
            case LEFT:
            case RIGHT:
                translate(win, trans[win->ev.key.keysym.sym - 1073741900].move);
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                init(win);
                break;
            case RESET:
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                x_rotation = 0;
                y_rotation = 0;
                scene->camera = (Vec3){0, 0, FOCAL_LEN};
                init(win);
                std::cout << "Reset" << std::endl;
                break;
            case KEY_0:
            case KEY_1:
            case KEY_2:
            case KEY_3:
            case KEY_4:
            case KEY_5:
            case KEY_6:
            case KEY_7:
            case KEY_8:
            case KEY_9:
                if (win->scene.objects[win->ev.key.keysym.sym - KEY_0])
                {
                    Global = win->scene.objects[win->ev.key.keysym.sym - KEY_0];
                    std::cout << "Select object: " << win->ev.key.keysym.sym - KEY_0 << std::endl;
                }
                else
                    std::cout << "index out of range" << std::endl;
                break;
            case NUM_0:
            case NUM_5:
            case NUM_2:
            case NUM_4:
            case NUM_6:
            case NUM_8:
                Global->center = Global->center + move_obj[NUM_0 - win->ev.key.keysym.sym];
                std::cout << "move with: " << move_obj[NUM_0 - win->ev.key.keysym.sym] << std::endl;
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                init(win);
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

float hit_sphere(Ray *ray, Vec3 center, float radius, float min, float max)
{
    Vec3 oc = ray->org - center;

    float a = length_squared(ray->dir);
    float half_b = dot(oc, ray->dir);
    float c = length_squared(oc) - radius * radius;
    float delta = half_b * half_b - a * c;
    if (delta < 0)
        return -1.0;
    delta = sqrtf(delta);
    float sol = (-half_b - delta) / a;
    if (sol <= min || sol >= max)
        sol = (-half_b + delta) / a;
    if (sol <= min || sol >= max)
        return -1.0;
    return (sol);
}

float hit_plan(Ray *ray, Vec3 normal, float d, float min, float max)
{
    float t = -d - dot(normal, ray->org);
    float div = dot(ray->dir, normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;
    return (t);
}

float hit_triangle(Ray *ray, Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, float min, float max)
{
    float t = dot(normal, (p1 - ray->org));
    float div = dot(normal, ray->dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;

    Vec3 p = point_at(ray, t) - p1;
    Vec3 u = p2 - p1;
    Vec3 v = p3 - p1;
    Vec3 w = normal / dot(normal, normal);
    float alpha = dot(cross(p, v), w);
    float beta = -dot(cross(p, u), w);
    if (alpha < 0 || beta < 0 || alpha + beta > 1.0)
        return -1.0;
    return t;
}

float hit_rectangle(Ray *ray, Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, float min, float max)
{
    float t = dot(normal, (p1 - ray->org));
    float div = dot(normal, ray->dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;

    float len = length(normal);
    Vec3 w = point_at(ray, t) - p1;
    Vec3 u = p2 - p1;
    Vec3 v = p3 - p1;
    float beta = -dot(cross(w, u), normal) / (len * len);
    float alpha = dot(cross(w, v), normal) / (len * len);
    if (alpha < 0 || beta < 0 || alpha > 1.0 || beta > 1.0)
        return -1.0;

    return t;
}

#if 1
float hit_cylinder(Ray *ray, Vec3 center, float radius, float height, Vec3 normal, float min, float max, Vec3 *hit_normal)
{
    Vec3 ran = {0, 0, 1};
    if (fabsf(dot(ran, normal)) >= 0.9f)
        ran = {1, 0, 0};
    Vec3 u = unit_vector(cross(ran, normal));
    Vec3 v = unit_vector(cross(normal, u));

    float a = pow2(dot(u, ray->dir)) + pow2(dot(v, ray->dir));
    float b = 2 * (dot((ray->org - center), u) * dot(u, ray->dir) + dot((ray->org - center), v) * dot(v, ray->dir));
    float c = pow2(dot((ray->org - center), u)) + pow2(dot((ray->org - center), v)) - pow2(radius);

    float delta = b * b - 4 * a * c;
    float x = -1.0;
    if (delta >= 0)
    {
        delta = sqrt(delta);
        // hit cylinder
        float x1 = (-b + delta) / (2 * a);
        float x2 = (-b - delta) / (2 * a);

        if (x1 < min || x1 > max)
            x1 = -1.0;
        if (x2 < min || x2 > max)
            x2 = -1.0;

        if (x1 == -1 || x2 < x1)
            x = x2;
        if (x2 == -1 || x1 < x2)
            x = x1;
        float cal = fabsf(dot(normal, point_at(ray, x) - center));
        if (cal > height / 2)
            x = -1.0;

        Vec3 p = point_at(ray, x);
        *hit_normal = p - (dot(p - center, normal) * normal + center);

    }

    Vec3 c1 = center + (height / 2) * normal;
    float d1 = hit_plan(ray, normal, -dot(c1, normal), min, max);
    if (d1 > 0)
    {
        Vec3 p1 = point_at(ray, d1);
        if (length(p1 - c1) <= radius && (x < 0 || d1 < x))
        {
            x = d1;
            *hit_normal = normal;
        }
    }

    Vec3 c2 = center - (height / 2) * normal;
    float d2 = hit_plan(ray, -1 * normal, -dot(c2, -1 * normal), min, max);
    if (d2 > 0)
    {
        Vec3 p1 = point_at(ray, d2);
        if (length(p1 - c2) <= radius && (x < 0 || d2 < x))
        {
            x = d2;
            *hit_normal = -1 * normal;
        }
    }
    if (x < min || x > max)
        return -1;
    return x;
}
#else

float hit_cylinder(Ray *ray, Obj cylin, float min, float max, Vec3 *hit_normal)
{
    Vec3 ran = (Vec3){0, 0, 1};
    if (fabsf(dot(ran, cylin.normal)) >= 0.9f)
        ran = (Vec3){1, 0, 0};
    Vec3 u = unit_vector(cross(ran, cylin.normal));
    Vec3 v = unit_vector(cross(cylin.normal, u));

    float a = pow2(dot(u, ray->dir)) + pow2(dot(v, ray->dir));
    float b = 2 * (dot(calc(ray->org, '-', cylin.center), u) * dot(u, ray->dir) + dot(calc(ray->org, '-', cylin.center), v) * dot(v, ray->dir));
    float c = pow2(dot(calc(ray->org, '-', cylin.center), u)) + pow2(dot(calc(ray->org, '-', cylin.center), v)) - pow2(cylin.radius);

    float delta = b * b - 4 * a * c;
    float x = -1.0;
    if (delta >= 0)
    {
        delta = sqrt(delta);
        float x1 = (-b + delta) / (2 * a);
        float x2 = (-b - delta) / (2 * a);

        if (x1 < min || x1 > max)
            x1 = -1.0;
        if (x2 < min || x2 > max)
            x2 = -1.0;

        if (x1 == -1 || x2 < x1)
            x = x2;
        if (x2 == -1 || x1 < x2)
            x = x1;
        float cal = fabsf(dot(cylin.normal, calc(point_at(ray, x), '-', cylin.center)));
        if (cal > cylin.height / 2)
            x = -1.0;
        Vec3 p = point_at(ray, x);
        float d = dot(calc(p, '-', cylin.center));
        *hit_normal = calc(calc(p, '-', scale(cylin.normal, '*', dot(calc(p, '-', cylin.center), cylin.normal))), '+', cylin.center);
    }

    Vec3 c1 = calc(cylin.center, '+', scale(cylin.normal, '*', cylin.height / 2));
    float d1 = hit_plan(ray, cylin.normal, -dot(c1, cylin.normal), min, max);
    if (d1 > 0)
    {
        Vec3 p1 = point_at(ray, d1);
        if (length(calc(p1, '-', c1)) <= cylin.radius && (x < 0 || d1 < x))
        {
            x = d1;
            *hit_normal = cylin.normal;
        }
    }

    Vec3 c2 = calc(cylin.center, '-', scale(cylin.normal, '*', cylin.height / 2));
    float d2 = hit_plan(ray, scale(cylin.normal, '*', -1), dot(c2, scale(cylin.normal, '*', -1)), min, max);
    if (d2 > 0)
    {
        Vec3 p1 = point_at(ray, d2);
        if (length(calc(p1, '-', c2)) <= cylin.radius && (x < 0 || d2 < x))
        {
            x = d2;
            *hit_normal = scale(cylin.normal, '*', -1);
        }
    }
    if (x < min || x > max)
        return -1;
    // printf("did hit cylinder\n");
    return x;
}
#endif

float hit_cone(Ray *ray, Vec3 center, Vec3 normal, float radius, float height, float min, float max)
{
    Vec3 ran = {0, 1, 0};
    if (fabsf(dot(ran, normal)) >= 0.9f)
        ran = {0, 0, 1};
    Vec3 u = unit_vector(cross(ran, normal));
    Vec3 v = unit_vector(cross(normal, u));

    float w = pow2(radius) / pow2(height);

    Vec3 org = ray->org - center;
#if 0
    float a = pow2(dot(ray->dir, u)) + pow2(dot(ray->dir, v)) - pow2(radius) * pow2(dot(ray->dir, normal)) / pow2(height);
    float b = 2 * dot(org, u) * dot(ray->dir, u) + 2 * dot(org, v) * dot(ray->dir, v) - ( w * 2 * dot(ray->dir, normal) * dot(org, normal)) + (2 * pow2(radius) / height) * dot(ray->dir, normal);
    float c = pow2(dot(org, u)) + pow2(dot(org, v)) - pow2(radius) - pow2(radius) * pow2(dot(org, normal)) / pow2(height) - (2 * pow2(radius) / height) * dot(org, normal);
#else
    float a = pow2(dot(ray->dir, u)) + pow2(dot(ray->dir, v)) - w * pow2(dot(ray->dir, normal));
    float b = 2 * dot(org, u) * dot(ray->dir, u) + 2 * dot(org, v) * dot(ray->dir, v) - (w * 2 * dot(org, normal) * dot(ray->dir, normal) - (2 * pow2(radius) / height) * dot(ray->dir, normal));
    float c = pow2(dot(org, u)) + pow2(dot(org, v)) - pow2(radius) - (w * pow2(dot(org, normal)) - (2 * pow2(radius) / height) * dot(org, normal));
#endif

    float delta = b * b - 4 * a * c;
    if (delta < 0)
        return -1.0;
    delta = sqrt(delta);
    // hit cylinder
    float x1 = (-b + delta) / (2 * a);
    float x2 = (-b - delta) / (2 * a);

    if (x1 < min || x1 > max)
        x1 = -1.0;
    if (x2 < min || x2 > max)
        x2 = -1.0;

    float x = -1;
    // height *= 3;
    float d1 = dot(normal, point_at(ray, x1) - center);
    float d2 = dot(normal, point_at(ray, x2) - center);
    if (x1 > 0 && d1 >= 0 && d1 < height)
        x = x1;
    if (x2 > 0 && d2 >= 0 && d2 < height && (x < 0 || x2 < x))
        x = x2;

#if 0
    Vec3 c1 = center + (height / 2) * normal;
    float d1 = hit_plan(ray, normal, -dot(c1, normal), min, max);
    if (d1 > 0)
    {
        Vec3 p1 = point_at(ray, d1);
        if (length(p1 - c1) <= radius && (x < 0 || d1 < x))
            x = d1;
    }

    Vec3 c2 = center - (height / 2) * normal;
    float d2 = hit_plan(ray, -1 * normal, -dot(c2, -1 * normal), min, max);
    if (d2 > 0)
    {
        Vec3 p1 = point_at(ray, d2);
        if (length(p1 - c2) <= radius && (x < 0 || d2 < x))
            x = d2;
    }
#endif
    if (x < min || x > max)
        return -1;
    return x;
}

Ray get_new_ray(Obj *obj, Ray *ray, float closest)
{
    Vec3 norm;
    float val;
    Ray nray;

    nray.org = point_at(ray, closest);
    switch (obj->type)
    {
    case sphere_:
        norm = unit_vector(ray->org - obj->center);
        break;
    case plan_:
    case triangle_:
    case rectangle_:
    case cylinder_:
    case cone_:
        norm = obj->normal;
        break;
    default:
        std::cerr << "Unkown object type" << std::endl;
        exit(1);
        break;
    }
    bool same_dir = (dot(norm, ray->dir) >= 0);
    if (same_dir)
        norm = -1 * norm;
    if (obj->mat == Refl_)
    {
        val = -2 * dot(ray->dir, norm);
        nray.dir = ray->dir + (val * norm);
    }
    else if (obj->mat == Refr_)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);
        float cos_theta = dot(ray->dir, norm) / (length(ray->dir) * length(norm));
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (refraction_ratio * sin_theta > 1.0) // Reflect
        {
            val = -2 * dot(ray->dir, norm);
            nray.dir = ray->dir + (val * norm);
        }
        else // Refract
        {
            Vec3 ray_dir = unit_vector(ray->dir);
            Vec3 Perp = refraction_ratio * (ray_dir - (dot(ray_dir, norm) * norm));
            Vec3 Para = sqrt(1 - pow(length(Perp), 2)) * (-1 * norm);
            nray.dir = Perp + Para;
        }
    }
    else if (obj->mat == Abs_)
        nray.dir = norm + random_unit_vector();
    else
    {
        std::cerr << "Error in get_new_ray" << std::endl;
        exit(-1);
    }
    return nray;
}

Vec3 light = {5, 5, 2};
// Vec3 light_dir = {-1, -1, 0};

Color ray_color(Win *win, Ray *ray, int max_depth)
{
    Scene *scene = &win->scene;
    Color ambient = (Color){.1, .2, .3};
    // ambient = (Color){};
    Color color = (Color){0};
    if (max_depth == 0)
        return color;
    // Color color = (Vec3){203.f / 255.999, 226.f / 255.999, 255.f / 255.999};

    float closest = FLT_MAX;
    int hit_index = -1;
    float x = 0.0;

    Vec3 normal;

    for (int i = 0; i < scene->pos; i++)
    {
        if (scene->objects[i]->type == plan_)
            x = hit_plan(ray, scene->objects[i]->normal, scene->objects[i]->d, ZERO, closest);
        if (scene->objects[i]->type == sphere_)
            x = hit_sphere(ray, scene->objects[i]->center, scene->objects[i]->radius, ZERO, closest);
        else if (scene->objects[i]->type == cylinder_)
            x = hit_cylinder(ray, scene->objects[i]->center, scene->objects[i]->radius, scene->objects[i]->height, scene->objects[i]->normal, ZERO, closest, &normal);
            // x = hit_cylinder(ray, *(scene->objects[i]), ZERO, closest, &normal);
        else if (scene->objects[i]->type == triangle_)
            x = hit_triangle(ray, scene->objects[i]->p1, scene->objects[i]->p2, scene->objects[i]->p3, scene->objects[i]->normal, ZERO, closest);
        else if (scene->objects[i]->type == rectangle_)
            x = hit_rectangle(ray, scene->objects[i]->p1, scene->objects[i]->p2, scene->objects[i]->p3, scene->objects[i]->normal, ZERO, closest);
        else if (scene->objects[i]->type == cone_)
            x = hit_cone(ray, scene->objects[i]->center, scene->objects[i]->normal, scene->objects[i]->radius, scene->objects[i]->height, ZERO, closest);
        if (x > 0.0)
        {
            hit_index = i;
            closest = x;
        }
    }
    if (hit_index != -1)
    {
#if 0
        Ray nray = get_new_ray(scene->objects[hit_index], ray, closest);
        float closest2 = FLT_MAX;
        int hit_index2 = -1;
        for (int i = 0; i < scene->pos; i++)
        {
            if (scene->objects[i]->type == plan_)
                x = hit_plan(&nray, scene->objects[i]->normal, scene->objects[i]->d, ZERO, closest2);
            if (scene->objects[i]->type == sphere_)
                x = hit_sphere(&nray, scene->objects[i]->center, scene->objects[i]->radius, ZERO, closest2);
            else if (scene->objects[i]->type == cylinder_)
                x = hit_cylinder(&nray, scene->objects[i]->center, scene->objects[i]->radius, scene->objects[i]->height, scene->objects[i]->normal, ZERO, closest2);
            else if (scene->objects[i]->type == triangle_)
                x = hit_triangle(&nray, scene->objects[i]->p1, scene->objects[i]->p2, scene->objects[i]->p3, scene->objects[i]->normal, ZERO, closest2);
            else if (scene->objects[i]->type == rectangle_)
                x = hit_rectangle(&nray, scene->objects[i]->p1, scene->objects[i]->p2, scene->objects[i]->p3, scene->objects[i]->normal, ZERO, closest2);
            else if (scene->objects[i]->type == cone_)
                x = hit_cone(&nray, scene->objects[i]->center, scene->objects[i]->normal, scene->objects[i]->radius, scene->objects[i]->height, ZERO, closest2);
            if (x > 0.0)
            {
                hit_index2 = i;
                closest2 = x;
            }
        }
        if (hit_index2 != -1)
        {
            Vec3 from = point_at(ray, closest);
            Vec3 to = point_at(&nray, closest2);
            float light_len = length(from - light);
            float obj_len = length(from - to);
            if (obj_len < light_len)
                color = color * scene->objects[hit_index2]->color;
            // return (Color){1,0,0};
            else
            {
                if (scene->objects[hit_index]->type == sphere_ || scene->objects[hit_index]->type == cone_)
                {
                    Vec3 normal = unit_vector(point_at(ray, closest) - scene->objects[hit_index]->center);
                    Vec3 light_dir = unit_vector(light - point_at(ray, closest));
                    float d = fmax(dot(normal, light_dir), 0);
                    color = color + d * (Vec3){1, 1, 1};
                }
                if (scene->objects[hit_index]->type == plan_)
                {
                    Vec3 normal = scene->objects[hit_index]->normal;
                    Vec3 light_dir = unit_vector(light - point_at(ray, closest));
                    float d = fmax(dot(normal, light_dir), 0);
                    color = color + d * (Vec3){1, 1, 1};
                }
            }
            color = color * scene->objects[hit_index]->color;
        }
        else
        {
            // float distance = dot(nray.org - point_at(ray->org, closest), light - point_at(ray, closest));
            if (scene->objects[hit_index]->type == sphere_ || scene->objects[hit_index]->type == cone_)
            {
                Vec3 normal = unit_vector(point_at(ray, closest) - scene->objects[hit_index]->center);
                Vec3 light_dir = unit_vector(light - point_at(ray, closest));
                float d = fmax(dot(normal, light_dir), 0);
                color = color + d * (Vec3){1, 1, 1};
            }
            if (scene->objects[hit_index]->type == plan_)
            {
                Vec3 normal = scene->objects[hit_index]->normal;
                Vec3 light_dir = unit_vector(light - point_at(ray, closest));
                float d = fmax(dot(normal, light_dir), 0);
                color = color + d * (Vec3){1, 1, 1};
            }
            color = color * scene->objects[hit_index]->color;
        }
#else

        Vec3 p = point_at(ray, closest);

        switch (scene->objects[hit_index]->type)
        {
        case sphere_:
        case cone_:
        {
            normal = p - scene->objects[hit_index]->center;
            break;
        }
        case cylinder_:
        {
            break;
        }
        case plan_:
        {
            normal = scene->objects[hit_index]->normal;
            break;
        }
        default:
        {
            printf("Error in ray color\n");
            exit(1);
        }
        }
        normal = unit_vector(normal);
        Vec3 light_dir = unit_vector(light - p);
        float d = fmax(dot(normal, light_dir), 0);

        color = ((Vec3){d, d, d} + ambient) * scene->objects[hit_index]->color;
#endif
    }
    else
        color = ambient;
    return color;
}

void TraceRay(Win *win)
{
    std::cout << "Tracing ray" << std::endl;
    Scene *scene = &win->scene;
    Color pixel;
    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            int rays_per_pixel = 32;
            pixel = (Color){0, 0, 0};
            for (int i = 0; i < rays_per_pixel; i++)
            {
                Vec3 pixel_center = scene->first_pixel + ((float)w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
                Vec3 dir = pixel_center - scene->camera;
                Ray ray = (Ray){.org = scene->camera, .dir = dir};
                pixel = pixel + ray_color(win, &ray, 2);
            }
            pixel = pixel / rays_per_pixel;
            if (pixel.x > 1)
                pixel.x = 1;
            if (pixel.y > 1)
                pixel.y = 1;
            if (pixel.z > 1)
                pixel.z = 1;
            win->pixels[h * win->width + w] = SDL_COLOR((int)(255.999 * sqrtf(pixel.x)), (int)(255.999 * sqrtf(pixel.y)), (int)(255.999 * sqrtf(pixel.z)));
        }
        std::cout << "remaining: " << win->height - h << std::endl;
    }
    std::cout << "render" << std::endl;
}

#if THREADS_LEN
void *TraceRay(void *arg)
{
    Multi *multi = (Multi *)arg;
    Win *win = multi->win;
    Scene *scene = &win->scene;
    while (1)
    {
        if (win->thread_finished[multi->idx])
        {
            usleep(1000);
            continue;
        }
        for (int h = multi->y_start; h < multi->y_end; h++)
        {
            for (int w = multi->x_start; w < multi->x_end; w++)
            {
                Vec3 pixel_center = scene->first_pixel + ((float)w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
                Vec3 dir = pixel_center - scene->camera;
                Ray ray = (Ray){.org = scene->camera, .dir = dir};
                Color pixel = ray_color(win, &ray, 2);
                win->scene.sum[h * win->width + w] = win->scene.sum[h * win->width + w] + pixel;
                pixel = win->scene.sum[h * win->width + w] / (float)frame_index;
                if (pixel.x > 1)
                    pixel.x = 1;
                if (pixel.y > 1)
                    pixel.y = 1;
                if (pixel.z > 1)
                    pixel.z = 1;
                win->pixels[h * win->width + w] = SDL_COLOR((int)(255.999 * sqrtf(pixel.x)), (int)(255.999 * sqrtf(pixel.y)), (int)(255.999 * sqrtf(pixel.z)));
            }
        }
        win->thread_finished[multi->idx] = 1;
    }
}
#endif
