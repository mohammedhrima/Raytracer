#include "/goinfre/mhrima/homebrew/opt/sdl2/include/SDL2/SDL.h"
#include <iostream>
#include <iomanip>
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

// R  G  B
// 24 16 8
#define COLOR(r, g, b) r << 24 | g << 16 | b << 8

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
// colors
#define Green 0x008000
#define Red 0xFF0000
#define Blue 0x0000FF
#define White 0xFFFFFF
#define ZERO .0001f

#ifndef FOCAL_LEN
#define FOCAL_LEN 2
#endif

#define THREADS 1

#if THREADS
#define THREADS_LEN 25
#else
#define THREADS_LEN 1
#endif

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
    Vec3 upv;
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
    _Atomic int thread_finished[THREADS_LEN];
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
    return min + (rand_pcg() * one_over_uint_max) * (max - min);
}
float degrees_to_radians(float degrees)
{
    return degrees * pi / 180.0;
}

Color color(float r, float g, float b)
{
    return (Color){r / 255.999, g / 255.999, b / 255.999};
}

// vectors calculations
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
    return v / f;
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
    Obj newobj = {};
    newobj.type = sphere_;
    newobj.center = center;
    newobj.radius = radius;
    newobj.color = color;
    newobj.mat = mat;
    return newobj;
}
Obj new_hemisphere(Vec3 center, float radius, Vec3 normal, Color color, Mat mat)
{
    Obj newobj = {};
    newobj.type = hemisphere_;
    newobj.center = center;
    newobj.radius = radius;
    newobj.color = color;
    newobj.mat = mat;
    newobj.heminormal = normal; // add_vec3(normal, center);
    return newobj;
}
Obj new_plan(Vec3 normal, float d, Color color, Mat mat)
{
    Obj newobj = {};
    newobj.type = plan_;
    newobj.normal = unit_vector(normal);
    newobj.d = d;
    newobj.color = color;
    newobj.mat = mat;
    return newobj;
}
Obj new_triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    Obj newobj = {};
    newobj.type = triangle_;
    newobj.p1 = p1;
    newobj.p2 = p2;
    newobj.p3 = p3;

    newobj.normal = cross_(unit_vector(p2 - p1), unit_vector(p3 - p1));
    newobj.color = color;
    newobj.mat = mat;
    return newobj;
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
    // Vec3 *normals = NULL;
    // Vec3 *textures = NULL;
    Obj *triangles = scene->objects;
    int numVertices = 0;
    // int numNormals = 0;
    // int numTextures = 0;
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

            // printf("newobj triangle: \n");
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

// SDL functions
Win *new_window(int width, int height, char *title)
{
    Win *win;
    win = (Win *)calloc(1, sizeof(Win));
    win->width = width;
    win->height = height;
    win->pixels = (uint32_t *)calloc(width * height, sizeof(uint32_t));
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

void clear_window(Win *win, int color)
{
    memset(win->pixels, color, win->width * win->height);
}
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
void draw_rect(Win *win, int x_start, int y_start, int width, int height, int color)
{
    for (int y = y_start; y < y_start + height; y++)
    {
        for (int x = x_start; x < x_start + width; x++)
        {
            // Calculate the index in the pixel buffer
            int index = y * win->width + x;
            // Set the pixel color to red (RGBA: 255, 0, 0, 255)
            win->pixels[index] = color;
        }
    }
}

// Raytracing
// hit functions
float hit_sphere(Obj sphere, Ray ray, float min, float max)
{

    Vec3 OC = ray.org - sphere.center;

    float a = length_squared(ray.dir);
    float half_b = dot(OC, ray.dir);
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

float hit_plan(Obj plan, Ray ray, float min, float max)
{
    float t = plan.d - dot(plan.normal, ray.org);
    float div = dot(ray.dir, plan.normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;
    return (t);
}

// TODO: check Barycentric Coordinates
float hit_triangle(Obj trian, Ray ray, float min, float max)
{
    float t = dot(trian.normal, (trian.p1 - ray.org));
    float div = dot(trian.normal, ray.dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;

    Vec3 p = point_at(ray, t) - trian.p1;
    Vec3 u = trian.p2 - trian.p1;
    Vec3 v = trian.p3 - trian.p1;
    Vec3 normal = cross_(u, v);
    Vec3 w = normal / dot(normal, normal);
    float alpha = dot(cross_(p, v), w);
    float beta = -dot(cross_(p, u), w);

    if (alpha < 0 || beta < 0 || alpha + beta > 1)
        return -1;

    return t;
}

float hit_hemisphere(Obj hemisphere, Ray ray, float min, float max)
{
    // hemisphere.color = (Color){hemisphere.color.b * va, hemisphere.color.g * va, hemisphere.color.b * va};
    Vec3 OC = ray.org - hemisphere.center;
    float a = length_squared(ray.dir);
    float half_b = dot(OC, ray.dir);
    float c = length_squared(OC) - hemisphere.radius * hemisphere.radius;
    float delta = half_b * half_b - a * c;
    if (delta < .0)
        return -1.0;
    float sq_delta = sqrtf(delta);
    float sol = (-half_b - sq_delta) / a;

    Vec3 p = unit_vector(point_at(ray, sol) - hemisphere.center);
    Vec3 axis1 = {-1, -1, 0};
    Vec3 axis2 = {-1, 1, 0};

    float d1 = dot(p, axis1);
    float d2 = dot(p, axis2);
    if (sol <= min || sol >= max || (d2 < 0 && d1 < 0))
    {
        sol = (-half_b + sq_delta) / a;
        p = unit_vector(point_at(ray, sol) - hemisphere.center);
        d1 = dot(p, axis1);
        d2 = dot(p, axis2);
        if (sol <= min || sol >= max || (d2 < 0 && d1 < 0))
            sol = -1;
    }
    return sol;
}
// rendering
Ray render_object(Obj obj, Ray ray, float closest)
{
    // point coordinates
    Vec3 cp_norm;
    Vec3 p = point_at(ray, closest);

    if (obj.type == sphere_)
        cp_norm = unit_vector(p - obj.center);
    if (obj.type == hemisphere_)
        cp_norm = unit_vector(p - obj.center);
    else if (obj.type == plan_ || obj.type == triangle_)
        cp_norm = obj.normal;

    bool same_dir = dot(cp_norm, ray.dir) >= 0;
    if (same_dir) // to be used when drawing triangle
        cp_norm = (Vec3){-cp_norm.x, -cp_norm.y, -cp_norm.z};
    Vec3 ranv = random_unit_vector();
    Vec3 ndir;
    if (obj.mat == Refl_)
    {
        float val;
        val = -2 * dot(ray.dir, cp_norm);
        ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
    }
    else if (obj.mat == Refr_)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);

        float cos_theta = dot(ray.dir, cp_norm) / (length(ray.dir) * length(cp_norm));
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (refraction_ratio * sin_theta > 1.0)
        {
            // Reflect
            float val;
            val = -2 * dot(ray.dir, cp_norm);
            ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
        }
        else
        {
            // Refract
            Vec3 ray_dir = unit_vector(ray.dir);
            Vec3 Perp = refraction_ratio * (ray_dir - (dot(ray_dir, cp_norm) * cp_norm));
            Vec3 Para = sqrt(1 - pow(length(Perp), 2)) * (-1 * cp_norm);
            ndir = Perp + Para;
        }
    }
    else if (obj.mat == Abs_)
        ndir = cp_norm + ranv;
    return (Ray){.org = p, .dir = ndir};
}

Color ray_color(Win *win, Ray ray, int depth)
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
            else if (scene->objects[i].type == hemisphere_)
                x = hit_hemisphere(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == plan_)
                x = hit_plan(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == triangle_)
                x = hit_triangle(scene->objects[i], ray, ZERO, closest);
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
            float a = 0.5 * (unit_vector(ray.dir).y + 1.0);
            float r = (1.0 - a) + a * .3;
            float g = (1.0 - a) + a * .7;
            float b = (1.0 - a) + a * 1.0;
            light = light + (attenuation * (Color){r, g, b});
            break;
        }
        if (attenuation.x <= ZERO && attenuation.y <= ZERO && attenuation.z <= ZERO)
            break;
    }
    return light;
}

extern int upaxis;

// Scene
float y_rotation = 0;
float x_rotation = 0;
float z_rotation = 0;
void rotate(Win *win, Vec3 &u, int axes, float angle);
void init(Win *win)
{
    Scene *scene = &win->scene;
    scene->view_angle = degrees_to_radians(60); // TODO: maybe it's useless
    /*
        translation: transl camera and cam_dir   (key board)
        rotation:    rotate cam_dir over camera, (listen_on_mouse)
    */
    // scene->cam_dir = rotate(degrees_to_radians(x_rotation), (Vec3){0, 0, -1}, 'y');
    // scene->cam_dir = rotate(degrees_to_radians(y_rotation), scene->cam_dir, 'x');
    // scene->upv = (Vec3){0, upaxis, 0};
    scene->cam_dir = (Vec3){0, 0, -1};
    rotate(win, scene->cam_dir, 'y', x_rotation);
    rotate(win, scene->cam_dir, 'x', y_rotation);
    scene->w = -1 * unit_vector(scene->cam_dir); // step in z axis and z

    // Vec3 upv = (Vec3){0, 1, 0}; // used for getting u,v
    // upv = rotate(degrees_to_radians(x_rotation), upv, 'y');
    // upv = rotate(degrees_to_radians(y_rotation), upv, 'x');
    scene->upv = (Vec3){0, 1, 0};

#if 1 // TODO: to be checked after, you remove it after
    rotate(win, scene->upv, 'y', x_rotation);
    rotate(win, scene->upv, 'x', y_rotation);
#endif

    scene->len = 1; // TODO: maybe it's useless
    float tang = tan(scene->view_angle / 2);
    float screen_height = 2 * tang * scene->len;
    float screen_width = screen_height * ((float)win->width / win->height);

    scene->u = unit_vector(cross_(scene->upv, scene->w)); // x+ (get v vector)
    scene->v = unit_vector(cross_(scene->w, scene->u));   // y+ (get u vector)

    // viewport steps
    scene->screen_u = screen_width * scene->u;
    scene->screen_v = -screen_height * scene->v;
    // window steps
    scene->pixel_u = scene->screen_u / win->width;
    scene->pixel_v = scene->screen_v / win->height;

    Vec3 screen_center = scene->camera + (-scene->len * scene->w);
    Vec3 upper_left = screen_center - (scene->screen_u + scene->screen_v) / 2;
    scene->first_pixel = upper_left + (scene->pixel_u + scene->pixel_v) / 2;
    // frame_index = 0;
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
void rotate(Win *win, Vec3 &u, int axes, float angle)
{
    // angle1 = (angle1 + (int)angle) % 360;
    // std::cout << "angle now " << angle1 << std::endl;

    float cos_ = cos(angle);
    float sin_ = sin(angle);
    switch (axes)
    {
    case 'x':
    {
        u = (Vec3){
            u.x,
            u.y * cos_ - u.z * sin_,
            u.y * sin_ + u.z * cos_};
        break;
    }
    case 'y':
    {
        u = (Vec3){
            u.x * cos_ + u.z * sin_,
            u.y,
            -u.x * sin_ + u.z * cos_,
        };
        break;
    }
    case 'z':
    {
        u = (Vec3){
            u.x * cos_ - u.y * sin_,
            u.x * sin_ + u.y * cos_,
            u.z};
        break;
    }
    default:
    {
        std::cerr << "Error in rotation" << std::endl;
        exit(0);
    }
    }
}

int flipe = -1;
int on_mouse_move(Win *win, int x, int y)
{
    Scene *scene = &win->scene;
    if (is_mouse_down)
    {
        int dx = x - old_x;
        int dy = y - old_y;
        // if (dx > 0)
        //     std::cout << "turn right: " << x_rotation << std::endl;
        // if (dx < 0)
        //     std::cout << "turn left: " << x_rotation << std::endl;

        // if (dy > 0)
        //     std::cout << "turn up: " << x_rotation << std::endl;
        // if (dy < 0)
        //     std::cout << "turn down: " << x_rotation << std::endl;
        if (win->scene.cam_dir.z > win->scene.camera.z)
        {
            x_rotation -= 0.1f * atan(dx / win->scene.len);
            y_rotation += (0.1f) * atan(dy / win->scene.len);
        }
        else
        {
            x_rotation -= 0.1f * atan(dx / win->scene.len);
            y_rotation -= (0.1f) * atan(dy / win->scene.len);
        }
        old_x = x;
        old_y = y;
    }
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
extern _Atomic(int) frame_index;
int is_mouse_down = 0;

void *TraceRay(void *arg)
{
    Multi *multi = (Multi *)arg;
    Win *win = multi->win;
    Scene *scene = &win->scene;
    int x_start, y_start, width, height;
    // struct timespec time_start, time_end;
    // unsigned int frame_time;
    divideWindow(win, multi->idx, x_start, y_start, width, height);
    while (1)
    {
        if (win->thread_finished[multi->idx])
        {
            usleep(1000);
            continue;
        }
        for (int h = y_start; h < y_start + height; h++)
        {
            for (int w = x_start; w < x_start + width; w++)
            {
                Vec3 pixel_center = scene->first_pixel + (w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
                Vec3 dir = pixel_center - scene->camera;
                Ray ray = (Ray){.org = scene->camera, .dir = dir};
                Color pixel = ray_color(win, ray, 5);
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
}

void add_objects(Win *win)
{
    struct
    {
        Vec3 normal;
        float dist; // distance from camera
        Mat mat;
    } plans[] = {
        {(Vec3){}, 0, (Mat)0},
        {(Vec3){0, -1, 0}, -4, Abs_}, // up
        {(Vec3){0, 1, 0}, -4, Abs_},  // down
        {(Vec3){0, 0, 1}, -12, Abs_}, // behind
        {(Vec3){1, 0, 0}, -4, Abs_},  // right
        {(Vec3){-1, 0, 0}, -4, Abs_}, // left
    };
    struct
    {
        Vec3 org;
        float rad;
        Mat mat;
    } spheres[] = {
        {(Vec3){0, 1, -2}, .5, Abs_},
        {(Vec3){-1, 0, -1}, .5, Abs_},
        {(Vec3){1, 0, -1}, .5, Abs_},
        {(Vec3){0, -.5, -2}, .5, Abs_},
        {(Vec3){0, -5, -.5}, .5, Abs_},
        {(Vec3){}, 0, (Mat)0},
    };
    Color colors[] = COLORS;
    int i = 0;
    while (spheres[i].mat)
    {
        Vec3 org = spheres[i].org;
        float rad = spheres[i].rad;
        Mat mat = spheres[i].mat;
        win->scene.objects[win->scene.pos] = new_sphere(org, rad, colors[win->scene.pos % (sizeof(colors) / sizeof(*colors))], mat);
        i++;
        win->scene.pos++;
    }
    i = 0;
    while (plans[i].mat)
    {
        Vec3 normal = plans[i].normal;
        float dist = plans[i].dist;
        Mat mat = plans[i].mat;
        win->scene.objects[win->scene.pos] = new_plan(normal, dist, colors[win->scene.pos % (sizeof(colors) / sizeof(*colors))], mat);
        if (i == 0)
        {
            win->scene.objects[win->scene.pos].light_intensity = 2;
            win->scene.objects[win->scene.pos].light_color = (Color){1, 1, 1};
        }
        i++;
        win->scene.pos++;
    }
}
_Atomic(int) frame_index;

int main()
{
    int width = 512;
    int height = width / 1;
    if (height < 1)
        height = 1;
    time_t time_start, time_end;
    Win *win = new_window(width, height, (char *)"Mini Raytracer");
    Scene *scene = &win->scene;
    scene->objects = (Obj *)calloc(100, sizeof(Obj));

    scene->camera = (Vec3){0, 0, FOCAL_LEN};
    scene->upv = (Vec3){0, 1, 0};

    sum = (Color *)calloc(win->width * win->height, sizeof(Color));
    init(win);
    frame_index = 1;
    int quit = 0;
    add_objects(win);

    for (int i = 0; i < THREADS_LEN; i++)
        win->thread_finished[i] = 1;

    Multi *multis[THREADS_LEN];
    for (int i = 0; i < THREADS_LEN; i++)
    {
        multis[i] = new_multi(i, win);
        pthread_create(&multis[i]->thread, nullptr, TraceRay, multis[i]);
    }
    struct
    {
        Vec3 move;
        char *msg;
    } trans[1000] = {
        [FORWARD - 1073741900] = {(Vec3){0, 0, -.5}, (char *)"forward"},
        [BACKWARD - 1073741900] = {(Vec3){0, 0, .5}, (char *)"backward"},
        [UP - 1073741900] = {(Vec3){0, .5, 0}, (char *)"up"},
        [DOWN - 1073741900] = {(Vec3){0, -.5, 0}, (char *)"down"},
        [LEFT - 1073741900] = {(Vec3){-.5, 0, 0}, (char *)"left"},
        [RIGHT - 1073741900] = {(Vec3){.5, 0, 0}, (char *)"right"},
    };

    while (!quit)
    {
        while (SDL_PollEvent(&win->ev) != 0)
        {
            switch (win->ev.type)
            {
            case QUIT:
                quit = true;
                break;
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
                    scene->upv = (Vec3){0, 1, 0};
                    frame_index = 1;
                    memset(sum, 0, win->width * win->height * sizeof(Color));
                    init(win);
                }
                break;
            case MOUSE_SCROLL:
                if (win->ev.wheel.y > 0)
                    translate(win, trans[FORWARD - 1073741900].move);
                else
                    translate(win, trans[BACKWARD - 1073741900].move);
                // scene->upv = (Vec3){0, 1, 0};
                frame_index = 1;
                memset(sum, 0, win->width * win->height * sizeof(Color));
                init(win);
                break;
            case PRESS:
                // case RELEASE:
                switch (win->ev.key.keysym.sym)
                {
                case FORWARD:
                case BACKWARD:
                case UP:
                case DOWN:
                case LEFT:
                case RIGHT:
                    translate(win, trans[win->ev.key.keysym.sym - 1073741900].move);
                    // scene->upv = (Vec3){0, 1, 0};
                    frame_index = 1;
                    memset(sum, 0, win->width * win->height * sizeof(Color));
                    init(win);
                    break;
                case ESC:
                    quit = true;
                    break;
                case RESET:
                    clear_window(win, 0);
                    frame_index = 1;
                    memset(sum, 0, win->width * win->height * sizeof(Color));
                    // scene->upv = (Vec3){0, 1, 0};
                    x_rotation = 0;
                    y_rotation = 0;
                    scene->camera = (Vec3){0, 0, FOCAL_LEN};
                    init(win);
                    std::cout << "Reset" << std::endl;
                    break;
                default:
                    std::cout << "key cliqued: " << win->ev.key.keysym.sym << std::endl;
                    break;
                }
                break;

            default:
                break;
            }
            // if draw is called
            // draw_rect(win, (int)x, (int)y, 20, 20, COLOR(255, 0, 0));
        }
        // set all threads to not finished
        for (int i = 0; i < THREADS_LEN; i++)
            win->thread_finished[i] = 0;
        time_start = get_time();
        while (1)
        {
            int finished = 1;

            for (int i = 0; i < THREADS_LEN; i++)
            {
                if (!win->thread_finished[i])
                    finished = 0;
            }
            if (finished)
                break;
            usleep(1000);
        }
        update_window(win);
        frame_index++;
        time_end = get_time();
        std::string dynamicTitle = std::to_string(time_end - time_start) + std::string(" ms");
        SDL_SetWindowTitle(win->window, dynamicTitle.c_str());
    }
    close_window(win);
}
