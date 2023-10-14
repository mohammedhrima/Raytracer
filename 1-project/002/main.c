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

typedef enum
{
    Reflectif_ = 11,
    Refractif_,
    Absorb_
} Mat;

typedef enum
{
    sphere_,
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
} Coor;

typedef Coor Col;

typedef struct
{
    Type type;
    Mat mat;
    Col color;
    union
    {
        // Sphere
        struct
        {
            Coor center;
            float radius;
        };
        // Plan
        struct
        {
            Coor vector1;
            Coor vector2;
            Coor point;
        };
    };
} Obj;

typedef struct
{
    Coor dir;
    Coor org;
} Ray;

typedef struct
{
    // rendering
    float focal_length;
    float aspect_ratio;
    float view_angle;
    Coor camera;
    Coor screen_u;
    Coor screen_v;
    Coor pixel_u;
    Coor pixel_v;
    Coor first_pixel;
    Coor u, v, w;
    Coor lookfrom;
    Coor lookat;
    Coor vup;
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
Coor coor(float x, float y, float z)
{
    return (Coor){.x = x, .y = y, .z = z};
}
Col color(float r, float g, float b)
{
    return (Col){.r = r / 255.999, .g = g / 255.999, .b = b / 255.999};
}
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
    return coor(t * v.x, t * v.y, t * v.z);
}
Coor mul(Coor leftv, Coor rightv)
{
    return coor(leftv.x * rightv.x, leftv.y * rightv.y, leftv.z * rightv.z);
}
Coor div_(Coor v, float t)
{
    if (t == 0)
    {
        printf("Error: dividing by 0\n");
        exit(1);
    }
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
Coor cross_(Coor u, Coor v)
{
    return coor(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}
Coor unit_vector(Coor v)
{
    float f = length(v);

    if (f <= 0.0001f)
        return (Coor){};
    return div_(v, f);
}
Coor random_vector(float min, float max)
{
    return coor(random_float(min, max), random_float(min, max), random_float(min, max));
}
Coor random_in_unit_sphere()
{
    while (1)
    {
        Coor v = random_vector(-1, 1);
        if (length_squared(v) <= 1)
            return v;
    }
}
Coor random_unit_vector()
{
    Coor u = random_in_unit_sphere();
    Coor v = unit_vector(u);
    return v;
}
Ray new_ray(Coor org, Coor dir)
{
    return (Ray){.dir = dir, .org = org};
}
Coor point_at(Ray ray, float t)
{
    return (Coor){.x = ray.org.x + t * ray.dir.x, .y = ray.org.y + t * ray.dir.y, .z = ray.org.z + t * ray.dir.z};
}
time_t get_time()
{
    struct timespec time_;
    clock_gettime(CLOCK_MONOTONIC, &time_);
    return (time_.tv_sec * 1000 + time_.tv_nsec / 1000000);
}

// mini raytracer
Obj new_sphere(Coor center, float radius, Col color, Mat mat)
{
    Obj new;
    new.center = center;
    new.radius = radius;
    new.color = color;
    new.type = sphere_;
    new.mat = mat;
    return new;
}

float hit_sphere(Obj sphere, Ray ray, float min, float max)
{
    Coor OC = sub_(ray.org, sphere.center);
    float a = length_squared(ray.dir);
    float half_b = dot(OC, ray.dir);
    float c = length_squared(OC) - sphere.radius * sphere.radius;
    float delta = half_b * half_b - a * c;
    if (delta < 0.0)
        return -1.0;
    float sq_delta = sqrtf(delta);
    float sol = (-half_b - sq_delta) / a;
    if (sol <= min || sol >= max)
        sol = (-half_b + sq_delta) / a;
    if (sol <= min || sol >= max)
        return -1.0;
    return (sol);
}

// TODO : to be verified
float reflectance(float cosine, float ref_idx)
{
    // Use Schlick's approximation for reflectance.
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

Ray render_sphere(Obj sphere, Ray ray, float closest)
{
    // point coordinates
    Coor p = point_at(ray, closest);
    Coor cp_norm = unit_vector(sub_(p, sphere.center));
    bool same_dir = dot(cp_norm, ray.dir) >= 0;
    if (same_dir) // to be used when drawing triangle
        cp_norm = (Coor){.x = -cp_norm.x, .y = -cp_norm.y, .z = -cp_norm.z};

    Coor ranv = random_unit_vector();
    Coor ndir;
    Ray nray;
    if (sphere.mat == Reflectif_)
    {
        float val;
        Coor ray_dir = unit_vector(ray.dir);
        val = -2 * dot(ray_dir, cp_norm);
        ndir = (Coor){.x = ray.dir.x + val * cp_norm.x, .y = ray.dir.y + val * cp_norm.y, .z = ray.dir.z + val * cp_norm.z};
    }
    if (sphere.mat == Refractif_)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);

        float cos_theta = dot(ray.dir, cp_norm) / (length(ray.dir) * length(cp_norm)); // fmin(dot(-unit_dir, normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_float(-FLT_MAX, FLT_MAX))
        {
            // Reflect
            float val;
            Coor ray_dir = unit_vector(ray.dir);
            val = -2 * dot(ray_dir, cp_norm);
            ndir = (Coor){.x = ray.dir.x + val * cp_norm.x, .y = ray.dir.y + val * cp_norm.y, .z = ray.dir.z + val * cp_norm.z};
        }
        else
        {
            // Refract
            Coor ray_dir = unit_vector(ray.dir);
            Coor Perp = mul_(refraction_ratio, sub_(ray_dir, mul_(dot(ray_dir, cp_norm), cp_norm)));
            Coor Para = mul_(sqrt(1 - pow(length(Perp), 2)), neg_(cp_norm));
            ndir = add_(Perp, Para);
        }
    }
    if (sphere.mat == Absorb_)
        ndir = add_(cp_norm, ranv);
    nray = new_ray(p, ndir);
    return nray;
}

Col ray_color(Win *win, Ray ray, int depth)
{
    Scene scene = win->scene;
    if (depth == 50)
        return coor(0, 0, 0);

    float closest = FLT_MAX;
    int hit_index = -1;
    float x = 0.0;
    for (int i = 0; i < scene.pos; i++)
    {
        if (scene.objects[i].type == sphere_)
            x = hit_sphere(scene.objects[i], ray, 0.0001f, closest);
        if (x > 0.0 && x < closest)
        {
            hit_index = i;
            closest = x;
        }
    }
    Ray nray;
    if (hit_index != -1)
    {
        if (scene.objects[hit_index].type == sphere_)
            nray = render_sphere(scene.objects[hit_index], ray, closest);

        Col color = ray_color(win, nray, depth + 1);
        color = mul(color, scene.objects[hit_index].color);
        return color;
    }

    float a = 0.5 * (unit_vector(ray.dir).y + 1.0);
    float r = (1.0 - a) + a * 0.3;
    float g = (1.0 - a) + a * 0.7;
    float b = (1.0 - a) + a * 1.0;
    return coor(r, g, b);
}


int draw(void *ptr)
{
    static int frame;
    static Col *sum;
    frame++;
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_MONOTONIC, &time_start);

    Win *win = (Win *)ptr;
    Scene *scene = (Scene *)(&win->scene);
    if (sum == NULL)
        sum = calloc(win->width * win->height, sizeof(Col));

#pragma omp parallel for
    for (int w = 0; w < win->width; w++)
    {
        for (int h = 0; h < win->height; h++)
        {
            Coor pixel_center = add_(add_(scene->first_pixel, mul_(w + random_float(0, 1), scene->pixel_u)), mul_(h + random_float(0, 1), scene->pixel_v));
            Coor dir = sub_(pixel_center, scene->camera);
            Ray ray = new_ray(scene->camera, dir);
            Col pixel = ray_color(win, ray, 0);
            sum[h * win->width + w] = add_(sum[h * win->width + w], pixel);
            pixel = div_(sum[h * win->width + w], (float)frame);
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
    scene->aspect_ratio = 16.0 / 9.0;
    win.width = 800;
    win.height = (int)((float)win.width / scene->aspect_ratio);
    if (win.height < 1)
        win.height = 1;

    scene->view_angle = degrees_to_radians(20);
    scene->lookfrom = coor(-2, 3, 20);
    scene->lookat = coor(0, 0, -1);
    scene->vup = coor(0, 1, 0);

    scene->camera = scene->lookfrom; // TODO: to be checked ?
    scene->focal_length = length(sub_(scene->lookfrom, scene->lookat));
    float tang = tan(scene->view_angle / 2);
    float screen_height = 2 * tang * scene->focal_length;
    float screen_width = screen_height * ((float)win.width / win.height);

    scene->w = unit_vector(sub_(scene->lookfrom, scene->lookat)); // lookfrom - lookat
    scene->u = unit_vector(cross_(scene->vup, scene->w));         // cross(vup,w)
#if 0
    scene->v = unit_vector(cross_(scene->w, scene->u)); // TODO: test it !
#else
    scene->v = cross_(scene->w, scene->u); // cross(w, u)
#endif
    // viewport steps
    scene->screen_u = mul_(screen_width, scene->u);
    scene->screen_v = mul_(screen_height, neg_(scene->v));
    // window steps
    scene->pixel_u = div_(scene->screen_u, win.width);
    scene->pixel_v = div_(scene->screen_v, win.height);

    Coor screen_center = sub_(scene->camera, mul_(scene->focal_length, scene->w));                   // camera - focal_length * w
    Coor upper_left = sub_(sub_(screen_center, div_(scene->screen_u, 2)), div_(scene->screen_v, 2)); // screen_center - screen_u / 2 - screen_v / 2
    scene->first_pixel = add_(upper_left, mul_(0.5, add_(scene->pixel_u, scene->pixel_v)));          // upper_left + 0.5 * pixel_u + 0.5 * pixel_v

    // add objects
    scene->pos = 0;
    scene->objects[scene->pos++] = new_sphere(coor(0.0, -100.5, -1.0), 100, coor(0.8, 0.8, 0.0), Absorb_);
    scene->objects[scene->pos++] = new_sphere(coor(0.0, 0.0, -1.0), 0.5, coor(0.7, 0.3, 0.3), Absorb_);     // center
    scene->objects[scene->pos++] = new_sphere(coor(-1.5, 0.0, -1.0), 0.5, coor(0.8, 0.8, 0.8), Reflectif_); // left
    scene->objects[scene->pos++] = new_sphere(coor(1.5, 0.0, -1.0), 0.5, coor(0.8, 0.6, 0.2), Refractif_);  // right

    win.mlx = mlx_init();
    win.win = mlx_new_window(win.mlx, win.width, win.height, "Mini Raytracer");
    win.img = mlx_new_image(win.mlx, win.width, win.height);
    win.addr = mlx_get_data_addr(win.img, &win.bits_per_pixel, &win.line_length, &win.endian);

    mlx_loop_hook(win.mlx, draw, &win);
    mlx_key_hook(win.win, listen, &win);
    mlx_loop(win.mlx);
}
