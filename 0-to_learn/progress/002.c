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
    Reflectif_ = 11,
    Refractif_,
    Absorb_
} Mat;

typedef enum
{
    sphere_,
    plan_,
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
        // Plan
        struct
        {
            Vec3 normal;
            float d;
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
    // float aspect_ratio;
    float view_angle;
    // Vec3 camera;
    Vec3 camera;
    Vec3 screen_u;
    Vec3 screen_v;
    Vec3 pixel_u;
    Vec3 pixel_v;
    Vec3 first_pixel;
    Vec3 u, v, w;
    Vec3 lookat;
    Vec3 vup;
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
// Ray tracing
float hit_sphere(Obj sphere, Ray ray, float min, float max)
{
    Vec3 OC = sub_vec3(ray.org, sphere.center);
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
    float t = -plan.d - dot(plan.normal, ray.org);
    float div = dot(ray.dir, plan.normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t < min || t > max)
        return -1.0;
    return (t);
}
// TODO : to be verified
float reflectance(float cosine, float ref_idx)
{
    // Use Schlick's approximation for reflectance.
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}
// TODO : to be optimized
Ray render_object(Obj obj, Ray ray, float closest)
{
    // point coordinates
    Vec3 p = point_at(ray, closest);
    Vec3 cp_norm;

    if (obj.type == sphere_)
        cp_norm = unit_vector(sub_vec3(p, obj.center));
    else if (obj.type == plan_)
        cp_norm = obj.normal;

    bool same_dir = dot(cp_norm, ray.dir) >= 0;
    if (same_dir) // to be used when drawing triangle
        cp_norm = (Vec3){-cp_norm.x, -cp_norm.y, -cp_norm.z};
    Vec3 ranv = random_unit_vector();
    Vec3 ndir;
    if (obj.mat == Reflectif_)
    {
        float val;
        val = -2 * dot(ray.dir, cp_norm);
        ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
    }
    else if (obj.mat == Refractif_)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);

        float cos_theta = dot(ray.dir, cp_norm) / (length(ray.dir) * length(cp_norm)); // fmin(dot(-unit_dir, normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_float(-FLT_MAX, FLT_MAX))
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
            Vec3 Perp = mul_vec3(refraction_ratio, sub_vec3(ray_dir, mul_vec3(dot(ray_dir, cp_norm), cp_norm)));
            Vec3 Para = mul_vec3(sqrt(1 - pow(length(Perp), 2)), mul_vec3(-1, cp_norm));
            ndir = add_vec3(Perp, Para);
        }
    }
    else if (obj.mat == Absorb_)
        ndir = add_vec3(cp_norm, ranv);
    return (Ray){.org = p, .dir = ndir};
}

int ray_hit(Win *win, Ray ray, float *closest)
{
    Scene scene = win->scene;
    int hit_index = -1;
    float x = .0;
    for (int i = 0; i < scene.pos; i++)
    {
        if (scene.objects[i].type == sphere_)
            x = hit_sphere(scene.objects[i], ray, ZERO, *closest);
        else if (scene.objects[i].type == plan_)
            x = hit_plan(scene.objects[i], ray, ZERO, *closest);
        if (x > .0 && x < *closest)
        {
            hit_index = i;
            *closest = x;
        }
    }
    return hit_index;
}

Color ray_color(Win *win, Ray ray, int depth)
{
    if (depth == 5)
        return (Color){0, 0, 0};
    Scene scene = win->scene;
    float closest = FLT_MAX;
    int hit_index = ray_hit(win, ray, &closest);
    if (hit_index != -1)
    {
        Obj *obj = &scene.objects[hit_index];
        Ray nray = render_object(*obj, ray, closest);
        Vec3 p = point_at(ray, closest);
        Vec3 normal;
        if (obj->type == sphere_)
            normal = unit_vector(sub_vec3(p, obj->center));
        else if (obj->type == plan_)
            normal = obj->normal;
        bool same_dir = dot(normal, ray.dir) >= 0;
        if (same_dir) // to be used when drawing triangle
            normal = (Vec3){-normal.x, -normal.y, -normal.z};
        normal = unit_vector(normal);

        float c = fmax(0, dot(mul_vec3(1, unit_vector(nray.dir)), normal));

        Color color = add_vec3(mul_vec3(c, mul_vec3_(ray_color(win, nray, depth + 1), obj->color)), mul_vec3(obj->light_intensity, obj->light_color));

        // color = mul_vec3_(color, obj->color);
        //  color = add_vec3(color, obj->color);
        return color;
    }

    float a = 0.5 * (unit_vector(ray.dir).y + 1.0);
    float r = (1.0 - a) + a * .3;
    float g = (1.0 - a) + a * .7;
    float b = (1.0 - a) + a * 1.0;
    return (Color){r, g, b};
}

int draw(void *ptr)
{
    static int frame;
    static Color *sum;
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    frame++;

    Win *win = (Win *)ptr;
    Scene *scene = &win->scene;
    if (sum == NULL)
        sum = calloc(win->width * win->height, sizeof(Color));
#pragma omp parallel for
    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            Vec3 pixel_center = add_vec3(add_vec3(scene->first_pixel, mul_vec3(w + random_float(0, 1), scene->pixel_u)), mul_vec3(h + random_float(0, 1), scene->pixel_v));
            Vec3 dir = sub_vec3(pixel_center, scene->camera);
            Ray ray = (Ray){.org = scene->camera, .dir = dir};
            Color pixel = ray_color(win, ray, 0);
            sum[h * win->width + w] = add_vec3(sum[h * win->width + w], pixel);
            pixel = div_vec3(sum[h * win->width + w], (float)frame);
            if (pixel.r > 1)
                pixel.r = 1;
            if (pixel.g > 1)
                pixel.g = 1;
            if (pixel.b > 1)
                pixel.b = 1;
            char *dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel / 8));
            *(unsigned int *)dst = ((int)(255.999 * sqrtf(pixel.r)) << 16) | ((int)(255.999 * sqrtf(pixel.g)) << 8) | ((int)(255.999 * sqrtf(pixel.b)));
        }
    }
    mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    float frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
    printf("%6.2f: render frame %d\n", frame_time, frame);
    return 0;
}

int main(void)
{
    Win win = {0};
    Scene *scene = &win.scene;
    win.width = 512;
    win.height = (int)((float)win.width / 1.0);
    if (win.height < 1)
        win.height = 1;

    scene->view_angle = degrees_to_radians(20);
    scene->camera = (Vec3){0, 0, 10};
    scene->lookat = (Vec3){0, 0, 0};
    scene->vup = (Vec3){0, 1, 0};

    scene->len = length(sub_vec3(scene->camera, scene->lookat));
    float tang = tan(scene->view_angle / 2);
    float screen_height = 2 * tang * scene->len;
    float screen_width = screen_height * ((float)win.width / win.height);

    scene->w = unit_vector(sub_vec3(scene->camera, scene->lookat)); // camera - lookat
    scene->u = unit_vector(cross_(scene->vup, scene->w));           // cross(vup,w)
    scene->v = unit_vector(cross_(scene->w, scene->u));             // cross(w, u)

    // viewport steps
    scene->screen_u = mul_vec3(screen_width, scene->u);
    scene->screen_v = mul_vec3(screen_height, mul_vec3(-1, scene->v));
    // window steps
    scene->pixel_u = div_vec3(scene->screen_u, win.width);
    scene->pixel_v = div_vec3(scene->screen_v, win.height);

    Vec3 screen_center = sub_vec3(scene->camera, mul_vec3(scene->len, scene->w));                                    // camera - len * w
    Vec3 upper_left = sub_vec3(sub_vec3(screen_center, div_vec3(scene->screen_u, 2)), div_vec3(scene->screen_v, 2)); // screen_center - screen_u / 2 - screen_v / 2
    scene->first_pixel = add_vec3(upper_left, mul_vec3(0.5, add_vec3(scene->pixel_u, scene->pixel_v)));              // upper_left + 0.5 * pixel_u + 0.5 * pixel_v

    // add objects
    scene->pos = 0;
    scene->objects[scene->pos++] = new_sphere((Vec3){.0, 0.5, 0.0}, .5, (Vec3){.7, .3, .3}, Absorb_); // center
    // scene->objects[scene->pos++] = new_sphere((Vec3){-1.5, .5, -1.0}, .5, (Vec3){.8, .8, .8}, Reflectif_); // left
    // scene->objects[scene->pos++] = new_sphere((Vec3){1.5, .5, -1.0}, .5, (Vec3){.2, .8, .8}, Reflectif_);  // right

    scene->objects[scene->pos++] = new_plan((Vec3){.0, 0.0, 1.0}, 5, (Vec3){0, .5, 0}, Absorb_); // behind
    scene->objects[scene->pos++] = new_plan((Vec3){.0, 1.0, .0}, -2, (Vec3){0, 0, .5}, Absorb_); // up
    scene->objects[scene->pos++] = new_plan((Vec3){.0, 1.0, .0}, 0.5, (Vec3){0, 0, .5}, Absorb_); // down
    scene->objects[scene->pos++] = new_plan((Vec3){1.0, .0, .0}, 2, (Vec3){.5, 0, 0}, Absorb_);  // left
    scene->objects[scene->pos++] = new_plan((Vec3){1.0, .0, .0}, -2, (Vec3){.5, 0, 0}, Absorb_); // right

    scene->objects[0].light_intensity = 50;
    scene->objects[0].light_color = (Color){1, .2, .2};

    win.mlx = mlx_init();
    win.win = mlx_new_window(win.mlx, win.width, win.height, "Mini Raytracer");
    win.img = mlx_new_image(win.mlx, win.width, win.height);
    win.addr = mlx_get_data_addr(win.img, &win.bits_per_pixel, &win.line_length, &win.endian);

    mlx_loop_hook(win.mlx, draw, &win);
    mlx_key_hook(win.win, listen, &win);
    mlx_loop(win.mlx);
}
