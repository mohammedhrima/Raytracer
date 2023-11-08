#include "../Libft/libft.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <mlx.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define ZERO .0001f
#define WIDTH 400
#define HEIGHT WIDTH
#define ESC 65307
#define RAYS_PER_PIXEL 25
#define PI 3.1415926535897932385

typedef enum s_type
{
    zero_,
    sphere_,
    plan_,
    cylinder_,
    cone_,
} t_type;

typedef struct s_vec3
{
    float x;
    float y;
    float z;
} t_vec3;
typedef t_vec3 t_color;

typedef struct s_ray
{
    t_vec3 dir;
    t_vec3 org;
} t_ray;

typedef struct s_obj
{
    t_type type;
    t_color color;
    t_vec3 normal;
    float d;
    t_vec3 center;
    float radius;
    float height;
} t_obj;

typedef struct s_scene
{
    float focal_len;
    t_vec3 light;
    // float light_ratio;
    t_vec3 light_color;
    // float ambient_light_ratio;
    t_vec3 ambient_light_color;
    t_vec3 camera;
    t_vec3 cam_dir;
    float fov;
    t_vec3 pixel_u;
    t_vec3 pixel_v;
    t_vec3 first_pixel;
    t_obj *objects;
    int obj_len;
    int pos;
    t_vec3 screen_u;
    t_vec3 screen_v;
    t_vec3 u;
    t_vec3 v;
    t_vec3 w;
    t_vec3 up;
    t_vec3 center;
    t_vec3 upper_left;
    float screen_width;
    float screen_height;
} t_scene;

typedef struct s_win
{
    int width;
    int height;
    void *mlx;
    void *win;
    void *img;
    void *title;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    t_scene scene;
} t_win;

typedef struct s_equ
{
    float a;
    float b;
    float c;
    float delta;
    float sol;
} t_equ;

// mlx
void close_window(t_win *win)
{
    mlx_destroy_window(win->mlx, win->win);
    exit(0);
}

int listen_on_key(int code, t_win *win)
{
    printf("%d\n", code);
    if (code == ESC)
        close_window(win);
    return (0);
}

int draw(void *ptr)
{
    t_win *win;

    win = (t_win *)ptr;
    mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
    return (0);
}

// utils
unsigned rand_pcg(void)
{
    static unsigned rng_state;
    unsigned state;
    unsigned word;

    state = rng_state;
    rng_state = rng_state * 747796405u + 2891336453u;
    word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return ((word >> 22u) ^ word);
}

float random_float(float min, float max)
{
    static double one_over_uint_max;

    one_over_uint_max = 1.0 / UINT_MAX;
    return (min + (rand_pcg() * one_over_uint_max) * (max - min));
}

float degrees_to_radians(float degrees)
{
    return (degrees * PI / 180.0);
}

float length_squared(t_vec3 v)
{
    return (pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

float length(t_vec3 v)
{
    return (sqrt(length_squared(v)));
}
float pow2(float x)
{
    return x * x;
}

t_vec3 point_at(t_ray *ray, float t)
{
    return ((t_vec3){.x = ray->org.x + t * ray->dir.x, .y = ray->org.y + t * ray->dir.y, .z = ray->org.z + t * ray->dir.z});
}

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
t_vec3 cross(t_vec3 u, t_vec3 v)
{
    return (t_vec3){u.y * v.z - u.z * v.y,
                    u.z * v.x - u.x * v.z,
                    u.x * v.y - u.y * v.x};
}
t_vec3 unit_vector(t_vec3 v)
{
    float f;

    f = length(v);
    if (f <= ZERO)
        return ((t_vec3){});
    return (scale(v, '/', f));
}

float dot(t_vec3 u, t_vec3 v)
{
    return (u.x * v.x + u.y * v.y + u.z * v.z);
}

void add_objects(t_win *win, t_obj obj)
{
    if (!win->scene.objects)
    {
        win->scene.obj_len = 100;
        win->scene.objects = ft_calloc(win->scene.obj_len, sizeof(t_obj));
    }
    win->scene.objects[win->scene.pos++] = obj;
    if (win->scene.pos + 4 == win->scene.obj_len)
    {
        t_obj *tmp = ft_calloc(win->scene.obj_len * 2, sizeof(t_obj));
        ft_memcpy(tmp, win->scene.objects, win->scene.obj_len * sizeof(t_obj));
        // free(win->scene.objects);
        win->scene.objects = tmp;
        win->scene.obj_len *= 2;
    }
}

t_obj new_sphere(t_vec3 center, float radius, t_color color)
{
    t_obj new = {0};
    new.type = sphere_;
    new.center = center;
    new.radius = radius;
    new.color = color;
    return new;
}

t_obj new_plan(t_vec3 normal, float d, t_color color)
{
    t_obj new = {0};
    new.type = plan_;
    new.normal = unit_vector(normal);
    new.d = d;
    new.color = color;
    return new;
}

t_obj new_cylinder(t_vec3 center, t_vec3 normal, float radius, float height, t_color color)
{
    t_obj new = {0};
    new.type = cylinder_;
    new.center = center;
    new.radius = radius;
    new.height = height;
    new.normal = unit_vector(normal);
    new.color = color;
    return new;
}

t_obj new_cone(t_vec3 center, float radius, float height, t_vec3 normal, t_color color)
{
    t_obj new = {0};
    new.type = cone_;
    new.center = center;
    new.radius = radius;
    new.height = height;
    new.normal = unit_vector(normal);
    new.color = color;
    return new;
}

// window
t_win *new_window(int height, int width)
{
    t_win *win;

    win = ft_calloc(1, sizeof(t_win));
    win->width = width;
    win->height = height;
    win->mlx = mlx_init();
    win->win = mlx_new_window(win->mlx, win->width, win->height,
                              "Mini Raytracer");
    win->img = mlx_new_image(win->mlx, win->width, win->height);
    win->addr = mlx_get_data_addr(win->img, &win->bits_per_pixel,
                                  &win->line_length, &win->endian);
    return (win);
}

void init_scene(t_win *win)
{
    win->scene.w = scale(unit_vector(win->scene.cam_dir), '*', -1);
    win->scene.up = (t_vec3){.x = 0, .y = 1, .z = 0};
    win->scene.screen_height = 2 * tan(win->scene.fov) * win->scene.focal_len;
    win->scene.screen_width = win->scene.screen_height * ((float)win->width / win->height);
    win->scene.u = unit_vector(cross(win->scene.up, win->scene.w));
    win->scene.v = unit_vector(cross(win->scene.w, win->scene.u));
    win->scene.screen_u = scale(win->scene.u, '*', win->scene.screen_width);
    win->scene.screen_v = scale(win->scene.v, '*', -win->scene.screen_height);
    win->scene.pixel_u = scale(win->scene.screen_u, '/', win->width);
    win->scene.pixel_v = scale(win->scene.screen_v, '/', win->height);
    win->scene.center = calc(win->scene.camera, '+', scale(win->scene.w, '*', -win->scene.focal_len));
    win->scene.upper_left = calc(win->scene.center, '-',
                                 scale(calc(win->scene.screen_u, '+', win->scene.screen_v), '/', 2));
    win->scene.first_pixel = calc(win->scene.upper_left, '+',
                                  scale(calc(win->scene.pixel_u, '+', win->scene.pixel_v), '/', 2));
}

float hit_sphere(t_ray *ray, t_obj sphere, float min, float max)
{
    t_vec3 oc;
    t_equ equ;

    oc = calc(ray->org, '-', sphere.center);
    equ.a = length_squared(ray->dir);
    equ.b = dot(oc, ray->dir);
    equ.c = length_squared(oc) - sphere.radius * sphere.radius;
    equ.delta = equ.b * equ.b - equ.a * equ.c;
    if (equ.delta < .0)
        return (-1.0);
    equ.delta = sqrtf(equ.delta);
    equ.sol = (-equ.b - equ.delta) / equ.a;
    if (equ.sol <= min || equ.sol >= max)
        equ.sol = (-equ.b + equ.delta) / equ.a;
    if (equ.sol <= min || equ.sol >= max)
        return (-1.0);
    return (equ.sol);
}

float hit_plan(t_ray *ray, t_obj plan, float min, float max)
{
    float t;
    float div;

    t = -plan.d - dot(plan.normal, ray->org);
    div = dot(ray->dir, plan.normal);
    if (fabsf(div) <= ZERO)
        return (-1.0);
    t /= div;
    if (t <= min || t >= max)
        return (-1.0);
    return (t);
}

float hit_cylinder(t_ray *ray, t_obj cylin, float min, float max, t_vec3 *hit_normal)
{
    // printf("call hit cyinder\n");
    t_vec3 ran = (t_vec3){0, 0, 1};
    if (fabsf(dot(ran, cylin.normal)) >= 0.9f)
        ran = (t_vec3){1, 0, 0};
    t_vec3 u = unit_vector(cross(ran, cylin.normal));
    t_vec3 v = unit_vector(cross(cylin.normal, u));

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
        t_vec3 p = point_at(ray, x);
        *hit_normal = calc(calc(p, '-', scale(cylin.normal, '*', dot(calc(p, '-', cylin.center), cylin.normal))), '+', cylin.center);
    }

    t_vec3 c1 = calc(cylin.center, '+', scale(cylin.normal, '*', cylin.height / 2));
    float d1 = hit_plan(ray, (t_obj){.normal = cylin.normal, .d = -dot(c1, cylin.normal)}, min, max);
    if (d1 > 0)
    {
        t_vec3 p1 = point_at(ray, d1);
        if (length(calc(p1, '-', c1)) <= cylin.radius && (x < 0 || d1 < x))
        {
            x = d1;
            *hit_normal = cylin.normal;
        }
    }

    t_vec3 c2 = calc(cylin.center, '-', scale(cylin.normal, '*', cylin.height / 2));
    float d2 = hit_plan(ray, (t_obj){.normal = scale(cylin.normal, '*', -1), .d = -dot(c2, scale(cylin.normal, '*', -1))}, min, max);
    if (d2 > 0)
    {
        t_vec3 p1 = point_at(ray, d2);
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

float hit_cone(t_ray *ray, t_obj cone, float min, float max)
{
    t_vec3 ran = (t_vec3){0, 1, 0};
    if (fabsf(dot(ran, cone.normal)) >= 0.9f)
        ran = (t_vec3){0, 0, 1};
    t_vec3 u = unit_vector(cross(ran, cone.normal));
    t_vec3 v = unit_vector(cross(cone.normal, u));

    float w = pow2(cone.radius) / pow2(cone.height);

    t_vec3 org = calc(ray->org, '-', cone.center);

    float a = pow2(dot(ray->dir, u)) + pow2(dot(ray->dir, v)) - w * pow2(dot(ray->dir, cone.normal));
    float b = 2 * dot(org, u) * dot(ray->dir, u) + 2 * dot(org, v) * dot(ray->dir, v) - (w * 2 * dot(org, cone.normal) * dot(ray->dir, cone.normal) - (2 * pow2(cone.radius) / cone.height) * dot(ray->dir, cone.normal));
    float c = pow2(dot(org, u)) + pow2(dot(org, v)) - pow2(cone.radius) - (w * pow2(dot(org, cone.normal)) - (2 * pow2(cone.radius) / cone.height) * dot(org, cone.normal));

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
    float d1 = dot(cone.normal, calc(point_at(ray, x1), '-', cone.center));
    float d2 = dot(cone.normal, calc(point_at(ray, x2), '-', cone.center));
    if (x1 > 0 && d1 >= 0 && d1 < cone.height)
        x = x1;
    if (x2 > 0 && d2 >= 0 && d2 < cone.height && (x < 0 || x2 < x))
        x = x2;
    if (x < min || x > max)
        return -1;
    return x;
}

t_color ray_color(t_win *win, t_ray ray)
{
    t_scene *scene = &win->scene;
    t_color color = (t_color){0, 0, 0}; // win->scene.ambient_light_color;
    t_vec3 light_dir;

    float closest = FLT_MAX;
    int hit_index = -1;
    t_vec3 normal;

    float x = 0.0;
    for (int i = 0; i < scene->pos; i++)
    {
        if (scene->objects[i].type == sphere_)
            x = hit_sphere(&ray, scene->objects[i], ZERO, closest);
        if (scene->objects[i].type == plan_)
            x = hit_plan(&ray, scene->objects[i], ZERO, closest);
        else if (scene->objects[i].type == cylinder_)
            x = hit_cylinder(&ray, scene->objects[i], ZERO, closest, &normal);
        else if (scene->objects[i].type == cone_)
            x = hit_cone(&ray, scene->objects[i], ZERO, closest);
        if (x > 0.0)
        {
            hit_index = i;
            closest = x;
        }
    }
    if (hit_index != -1)
    {
        if (scene->objects[hit_index].type == sphere_)
            normal = calc(point_at(&ray, closest), '-', scene->objects[hit_index].center);
        else if (scene->objects[hit_index].type == plan_)
            normal = scene->objects[hit_index].normal;
#if 1
        normal = unit_vector(normal);
        light_dir = unit_vector(calc(win->scene.light, '-', point_at(&ray, closest)));
        float d = fmax(dot(normal, light_dir), 0);
        color = calc(calc((t_color){d, d, d}, '+', win->scene.ambient_light_color), '*', scene->objects[hit_index].color);
#else
        color = scene->objects[hit_index].color;
#endif
    }
    else
        color = win->scene.ambient_light_color;
    return color;
}

t_color check_pixel(t_color pixel)
{
    pixel = scale(pixel, '/', RAYS_PER_PIXEL);
    if (pixel.x > 1)
        pixel.x = 1;
    if (pixel.y > 1)
        pixel.y = 1;
    if (pixel.z > 1)
        pixel.z = 1;
    return (pixel);
}

t_color get_color(t_win *win, t_scene *scene, int w, int h)
{
    int i;
    t_color pixel;
    t_vec3 pixel_center;
    t_vec3 dir;
    t_ray ray;

#if 1
    pixel = (t_color){.x = 0, .y = 0, .z = 0};
    i = 0;
    while (i < RAYS_PER_PIXEL)
    {
        pixel_center = calc(calc(scene->first_pixel, '+', scale(scene->pixel_u, '*', w + random_float(0, 1))), '+', scale(scene->pixel_v, '*', h + random_float(0, 1)));
        dir = calc(pixel_center, '-', scene->camera);
        ray = (t_ray){.org = scene->camera, .dir = dir};
        pixel = calc(pixel, '+', ray_color(win, ray));
        i++;
    }
#else
    pixel = (t_color){1, .5, 0};
#endif
    return (check_pixel(pixel));
}

unsigned int sqrt_color(t_color pixel)
{
    return (((int)(255.999 * sqrtf(pixel.x)) << 16) | ((int)(255.999 * sqrtf(pixel.y)) << 8) | ((int)(255.999 * sqrtf(pixel.z))));
}

void set_pixels(t_win *win)
{
    int h;
    int w;
    t_color pixel;
    char *dst;

    h = 0;
    w = 0;
    while (h < win->height)
    {
        w = 0;
        while (w < win->width)
        {
            pixel = get_color(win, &win->scene, w, h);
            dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel / 8));
#if 1
            *(unsigned int *)dst = sqrt_color(pixel);
#else
            *(unsigned int *)dst = (((int)(255.999 * (pixel.x)) << 16) | ((int)(255.999 * (pixel.y)) << 8) | ((int)(255.999 * (pixel.z))));
#endif
            w++;
        }
        // printf("remaining: %d\n", win->height - h);
        h++;
    }
    printf("render\n");
}

int skip(char *str, char c, int i)
{
    if (str[i] == '\0')
    {
        printf("Error unexpected EOL\n");
        exit(1);
    }
    if (str[i] != c)
    {
        printf("Error 0 unexpected '%c'\n", str[i]);
        exit(1);
    }
    i++;
    if (str[i] == '\0')
    {
        printf("Error unexpected EOL\n");
        exit(1);
    }
    return i;
}

float parse_number(char *str, int *ptr)
{
    int i = *ptr;
    float res = 0.0;
    int sign = 1;

    while (str[i] && ft_isspace(str[i]))
        i++;
    if (ft_strchr("+-", str[i]))
    {
        if (str[i] == '-')
            sign = -1;
        i = skip(str, str[i], i);
    }
    if (!ft_isdigit(str[i]))
    {
        printf("Error 1 unexpected '%c'\n", str[i]);
        exit(1);
    }
    while (ft_isdigit(str[i]))
    {
        res = 10 * res + sign * (str[i] - '0');
        i++;
    }
    if (str[i] == '.')
    {
        i++;
        float j = 0.1;
        while (ft_isdigit(str[i]))
        {
            res = res + sign * j * (str[i] - '0');
            j /= 10;
            i++;
        }
    }
    while (str[i] && ft_isspace(str[i]))
        i++;
    *ptr = i;
    return res;
}

void parse_error(float num, float min, float max, char *msg)
{
    if (num < min || num > max)
    {
        printf("Error: %s should be in range [%f,%f]\n", msg, min, max);
        exit(1);
    }
}

t_vec3 parse_color(char *str, int *i)
{
    t_vec3 color;
    color.x = parse_number(str, i);
    *i = skip(str, ',', *i);
    color.y = parse_number(str, i);
    *i = skip(str, ',', *i);
    color.z = parse_number(str, i);
    parse_error(color.x, 0, 255, "RGB");
    parse_error(color.y, 0, 255, "RGB");
    parse_error(color.z, 0, 255, "RGB");
    color.x /= 255.999;
    color.y /= 255.999;
    color.z /= 255.999;
    return color;
}

t_vec3 parse_normal(char *str, int *i)
{
    t_vec3 normal;

    normal.x = parse_number(str, i);
    *i = skip(str, ',', *i);
    normal.y = parse_number(str, i);
    *i = skip(str, ',', *i);
    normal.z = parse_number(str, i);
    parse_error(normal.x, -1, 1, "normal");
    parse_error(normal.y, -1, 1, "normal");
    parse_error(normal.z, -1, 1, "normal");
    return normal;
}

void parse_ambient_light(t_win *win, char *elems_str, char *str, int i)
{
    if (elems_str[str[i]])
    {
        // free(elems_str);
        printf("Error: ambient light already exists\n");
        exit(1);
    }
    elems_str[str[i++]] = 1;
    float ambient_light_ratio = parse_number(str, &i);
    parse_error(ambient_light_ratio, 0, 1, "ambient light ratio");
    win->scene.ambient_light_color = parse_color(str, &i);
    if (str[i])
    {
        printf("Error 2 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("A , %f, %f, %f, %f\n", ambient_light_ratio, win->scene.ambient_light_color.x,
           win->scene.ambient_light_color.y, win->scene.ambient_light_color.z);
    win->scene.ambient_light_color = scale(win->scene.ambient_light_color, '*', ambient_light_ratio);
}

void parse_camera(t_win *win, char *elems_str, char *str, int i)
{
    if (elems_str[str[i]])
    {
        // free(elems_str);
        printf("Error: camera already exists\n");
        exit(1);
    }
    elems_str[str[i++]] = 1;
    win->scene.camera.x = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.camera.y = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.camera.z = parse_number(str, &i);
    win->scene.cam_dir = parse_normal(str, &i);
    win->scene.fov = parse_number(str, &i);
    parse_error(win->scene.fov, 0, 180, "fov");
    win->scene.fov = degrees_to_radians(win->scene.fov) / 2;
    // win->scene.focal_len = length(win->scene.camera);
    win->scene.focal_len = 1;

    if (str[i])
    {
        // free(elems_str);
        printf("Error 3 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("C %f, %f, %f  %f, %f, %f  %f\n", win->scene.camera.x, win->scene.camera.y,
           win->scene.camera.z, win->scene.cam_dir.x, win->scene.cam_dir.y,
           win->scene.cam_dir.z, win->scene.fov);
}

void parse_light(t_win *win, char *elems_str, char *str, int i)
{
    if (elems_str[str[i]])
    {
        // free(elems_str);
        printf("Error: light already exists\n");
        exit(1);
    }
    elems_str[str[i++]] = 1;
    win->scene.light.x = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.light.y = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.light.z = parse_number(str, &i);
    float light_ratio = parse_number(str, &i);
    parse_error(light_ratio, 0, 1, " light brightness");

    if (str[i])
    {
        // free(elems_str);
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("L, %f, %f, %f  %f %f, %f, %f \n", win->scene.light.x, win->scene.light.y,
           win->scene.light.z, light_ratio, win->scene.light_color.x, win->scene.light_color.y,
           win->scene.light_color.z);
    win->scene.light_color = scale((t_vec3){1, 1, 1}, '*', light_ratio);
}

void parse_sphere(t_win *win, char *str, int i)
{
    t_vec3 center;
    i += 2;
    center.x = parse_number(str, &i);
    i = skip(str, ',', i);
    center.y = parse_number(str, &i);
    i = skip(str, ',', i);
    center.z = parse_number(str, &i);
    float radius = parse_number(str, &i) / 2;
    t_color color = parse_color(str, &i);
    add_objects(win, new_sphere(center, radius, color));

    if (str[i])
    {
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("sp, %f, %f, %f  %f %f, %f, %f \n", center.x, center.y, center.z,
           win->scene.objects[win->scene.pos - 1].radius * 2,
           win->scene.objects[win->scene.pos - 1].color.x,
           win->scene.objects[win->scene.pos - 1].color.y,
           win->scene.objects[win->scene.pos - 1].color.z);
}

void parse_plan(t_win *win, char *str, int i)
{
    t_vec3 point;
    t_vec3 normal;
    i += 2;
    point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    point.z = parse_number(str, &i);
    normal = parse_normal(str, &i);
    add_objects(win, new_plan(normal, dot(point, normal), parse_color(str, &i)));
    if (str[i])
    {
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("pl, %f, %f, %f  %f, %f, %f %f, %f, %f \n", point.x, point.y, point.z, normal.x, normal.y, normal.z,
           win->scene.objects[win->scene.pos - 1].color.x,
           win->scene.objects[win->scene.pos - 1].color.y,
           win->scene.objects[win->scene.pos - 1].color.z);
}

void parse_cylinder(t_win *win, char *str, int i)
{
    i += 2;
    t_vec3 point;

    point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    point.z = parse_number(str, &i);
    t_vec3 normal = parse_normal(str, &i);
    float radius = parse_number(str, &i) / 2;
    float height = parse_number(str, &i);
    add_objects(win, new_cylinder(point, normal, radius, height, parse_color(str, &i)));
    if (str[i])
    {
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("cy, %f, %f, %f  %f, %f, %f  %f %f  %f, %f, %f \n", point.x, point.y, point.z,
           win->scene.objects[win->scene.pos - 1].normal.x, win->scene.objects[win->scene.pos - 1].normal.y,
           win->scene.objects[win->scene.pos - 1].normal.z, win->scene.objects[win->scene.pos - 1].radius * 2,
           win->scene.objects[win->scene.pos - 1].height, win->scene.objects[win->scene.pos - 1].color.x,
           win->scene.objects[win->scene.pos - 1].color.y, win->scene.objects[win->scene.pos - 1].color.z);
}

void parse_line(t_win *win, char *str, char *elems_str)
{
    int i = 0;
    int require = 0;

    while (str && str[i])
    {
        if (str[i] && ft_isspace(str[i]))
            i++;
        else if (str[i] == 'A')
            return parse_ambient_light(win, elems_str, str, i);
        else if (str[i] == 'C')
            return parse_camera(win, elems_str, str, i);
        else if (str[i] == 'L')
            return parse_light(win, elems_str, str, i);
        else if (ft_strstr(str + i, "sp") == str + i)
            return parse_sphere(win, str, i);
        else if (ft_strstr(str + i, "pl") == str + i)
            return parse_plan(win, str, i);
        else if (ft_strstr(str + i, "cy") == str + i)
            return parse_cylinder(win, str, i);
        else
        {
            printf("Error\n");
            exit(1);
        }
    }
}

void parse_file(t_win *win, int argc, char **argv)
{
    char *elems_str;
    elems_str = ft_calloc(255, 1);
    if (argc != 2 || ft_strlen(argv[1]) < 3 || ft_strcmp(argv[1] + ft_strlen(argv[1]) - 3, ".rt"))
    {
        printf("executable require .rt file as argument\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        printf("Error: opening file\n");
        exit(1);
    }
    char *str = ft_readline(fd);
    while (str)
    {
        printf("parse :%s\n\n", str);
        if (str[0] != '#')
            parse_line(win, str, elems_str);
        str = ft_readline(fd);
    }
}

int main(int ac, char **av)
{
    t_win *win;
    win = new_window(HEIGHT, WIDTH);

    parse_file(win, ac, av);
#if 0
    // win->scene.ambient_light_color = (t_color){.x = 0, .y = 0, .z = 0};
    win->scene.ambient_light_color = (t_color){.x = .5, .y = .4, .z = .3};
    win->scene.camera = (t_vec3){.x = 0, .y = 0, .z = 1};
    win->scene.cam_dir = (t_vec3){.x = 0, .y = 0, .z = -1};
    win->scene.focal_len = length(win->scene.camera);
    win->scene.fov = degrees_to_radians(40) / 2;
    win->scene.light = (t_vec3){10, 10, 10};
    win->scene.light_color = scale((t_vec3){1, 1, 1}, '*', 0.6);

    // add_objects(win, new_plan((t_vec3){0, 1, 0}, 5, (t_color){255. / 225.999, 0, 128. / 255.999}));
    // add_objects(win, new_plan((t_vec3){0, -1, 0}, 5, (t_color){0, 255. / 255.999, 0}));
    // add_objects(win, new_plan((t_vec3){1, 0, 0}, 5, (t_color){255. / 255.999, 0, 0}));
    // add_objects(win, new_plan((t_vec3){-1, 0, 0}, 5, (t_color){0, 255. / 255.999, 128. / 255.999}));
    // add_objects(win, new_plan((t_vec3){0, 0, 1}, 40, (t_color){210. / 255.999, 25. / 255.999, 255. / 255.999}));
    // add_objects(win, new_cylinder((t_vec3){0, 0, -10}, (t_vec3){0, 1, -1}, 1, 3, (t_color){210. / 255.999, 25. / 255.999, 255. / 255.999}));
#endif
    init_scene(win);
    set_pixels(win);
    mlx_hook(win->win, 2, 1L << 0, listen_on_key, win);
    mlx_loop_hook(win->mlx, draw, win);
    mlx_loop(win->mlx);

    close_window(win);
}
