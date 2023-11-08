#include "header.h"

void *ft_memset(void *pointer, int c, size_t len)
{
    size_t i;
    unsigned char *destionation;

    destionation = (unsigned char *)pointer;
    i = 0;
    while (i < len)
    {
        *destionation = (unsigned char)c;
        destionation++;
        i++;
    }
    return (pointer);
}
void *ft_memcpy(void *destination, void *source, size_t len)
{
    size_t i;
    char *pointer1;
    char *pointer2;

    if (!destination)
        return (source);
    if (!source)
        return (destination);
    pointer1 = (char *)destination;
    pointer2 = (char *)source;
    i = 0;
    while (i < len)
    {
        pointer1[i] = pointer2[i];
        i++;
    }
    return (destination);
}

// void *ft_calloc(size_t count, size_t size)
// {
//     void *array;

//     if (count && size > SIZE_MAX / count)
//         return (NULL);
//     array = (void *)malloc(count * size);
//     if (!array)
//     {
//         printf("Error allocation in calloc\n");
//         exit(-1);
//     }
//     ft_memset(array, 0, count * size);
//     return (array);
// }

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
t_vec3 unit_vector(t_vec3 v)
{
    float f;

    f = length(v);
    if (f <= ZERO)
        return ((t_vec3){});
    return (scale(v, '/', f));
}

t_vec3 random_unit_vector(void)
{
    t_vec3 u;
    t_vec3 v;

    while (1)
    {
        u = (t_vec3){.x = random_float(-1, 1), .y = random_float(-1, 1), .z = random_float(-1, 1)};
        if (length_squared(u) <= 1)
            break;
    }
    v = unit_vector(u);
    return (v);
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
t_vec3 cross(t_vec3 u, t_vec3 v)
{
    return (t_vec3){u.y * v.z - u.z * v.y,
                    u.z * v.x - u.x * v.z,
                    u.x * v.y - u.y * v.x};
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

float unit_dot(t_vec3 u, t_vec3 v)
{
    return (u.x * v.x + u.y * v.y + u.z * v.z);
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

t_obj new_cylinder(t_vec3 center, float radius, t_vec3 normal, t_color color)
{
    t_obj new = {0};
    new.type = cylinder_;
    new.center = center;
    new.radius = radius;
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
    new.normal = normal;
    new.color = color;
    return new;
}
void add_objects(t_win *win)
{
    // TODO: protect it from heap oveflow
    int len = 100;
    win->scene.objects = ft_calloc(len, sizeof(t_obj));
    struct
    {
        t_vec3 org;
        float rad;
    } spheres[] = {
        {(t_vec3){.x = 1, .y = 1, .z = -1}, .5},
        {(t_vec3){.x = -.5, .y = -.5, .z = -1}, .5},
        {(t_vec3){.x = -1, .y = 1, .z = -1}, .5},
        {(t_vec3){.x = 1, .y = -1, .z = -1}, .5},
        {(t_vec3){.x = 0.5, .y = 0, .z = -1}, .5},
        {(t_vec3){}, -1},
    };
    t_color colors[] = {
        (t_color){.x = 0.92, .y = 0.19, .z = 0.15},
        (t_color){.x = 0.42, .y = 0.92, .z = 0.72},
        (t_color){.x = 0.42, .y = 0.87, .z = 0.92},
        (t_color){.x = 0.30, .y = 0.92, .z = 0.64},
        (t_color){.x = 0.39, .y = 0.92, .z = 0.63},
        (t_color){.x = 0.42, .y = 0.92, .z = 0.80},
        (t_color){.x = 0.47, .y = 0.16, .z = 0.92},
        (t_color){.x = 0.42, .y = 0.58, .z = 0.92},
        (t_color){.x = 0.92, .y = 0.40, .z = 0.30},
        (t_color){.x = 0.61, .y = 0.75, .z = 0.24},
        (t_color){.x = 0.83, .y = 0.30, .z = 0.92},
        (t_color){.x = 0.23, .y = 0.92, .z = 0.08},
    };
    int i = 0;
    while (spheres[i].rad > 0)
    {
        t_vec3 org = spheres[i].org;
        float rad = spheres[i].rad;
        win->scene.objects[win->scene.pos] = new_sphere(org, rad, colors[win->scene.pos % (sizeof(colors) / sizeof(*colors))]);
        if (win->scene.pos + 4 == len)
        {
            t_obj *tmp = ft_calloc(len * 2, sizeof(t_obj));
            ft_memcpy(tmp, win->scene.objects, len * sizeof(t_obj));
            free(win->scene.objects);
            win->scene.objects = tmp;
            len *= 2;
        }
        win->scene.pos++;
        i++;
    }
#if 0
    struct
    {
       t_vec3 normal;
        float dist; // distance from camera
    } plans[] = {
        {(Vec3){.x = 0, .y = -1, .z = 0}, -4}, // up
        {(Vec3){.x = 0, .y = 1, .z = 0}, -4},  // down
        {(Vec3){.x = 0, .y = 0, .z = 1}, -12}, // behind
        {(Vec3){.x = 1, .y = 0, .z = 0}, -4},  // right
        {(Vec3){.x = -1, .y = 0, .z = 0}, -4}, // left
        {(Vec3){}, 0},
    };
    i = 0;
    while (plans[i].mat)
    {
       t_vec3 normal = plans[i].normal;
        float dist = plans[i].dist;
        Mat mat = plans[i].mat;
        win.scene.objects[win.scene.pos] = new_plan(normal, dist, colors[win.scene.pos % (sizeof(colors) / sizeof(*colors))], mat);
        if (i == 0)
        {
            win.scene.objects[win.scene.pos].light_intensity = 1;
            win.scene.objects[win.scene.pos].light_color = (Color){1, 1, 1};
        }
        i++;
        win.scene.pos++;
    }
#endif
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

    pixel = (t_color){.x = 0, .y = 0, .z = 0};
    i = 0;
    while (i < RAYS_PER_PIXEL)
    {
        pixel_center = calc(calc(scene->first_pixel, '+', scale(scene->pixel_u, '*', w + random_float(0, 1))), '+',
                            scale(scene->pixel_v, '*', h + random_float(0, 1)));
        dir = calc(pixel_center, '-', scene->camera);
        ray = (t_ray){.org = scene->camera, .dir = dir};
        pixel = calc(pixel, '+', ray_color(win, ray));
        i++;
    }
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
            *(unsigned int *)dst = sqrt_color(pixel);
            w++;
        }
        printf("remaining: %d\n", win->height - h);
        h++;
    }
}

// ray tracing
float hit_sphere(t_ray *ray, t_obj sphere, float min, float max)
{
    t_vec3 oc;
    t_equ equ;

    oc = calc(ray->org, '-', sphere.center);
    equ.a = length_squared(ray->dir);
    equ.b = unit_dot(oc, ray->dir);
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

    t = plan.d - unit_dot(plan.normal, ray->org);
    div = unit_dot(ray->dir, plan.normal);
    if (fabsf(div) <= ZERO)
        return (-1.0);
    t /= div;
    if (t <= min || t >= max)
        return (-1.0);
    return (t);
}

float hit_cylinder(t_ray *ray, t_obj cylin, float min, float max)
{
    t_vec3 ran = (t_vec3){0, 0, 1};
    if (fabsf(unit_dot(ran, cylin.normal)) >= 0.9f)
        ran = (t_vec3){1, 0, 0};
    t_vec3 u = unit_vector(cross(ran, cylin.normal));
    t_vec3 v = unit_vector(cross(cylin.normal, u));

    float a = pow2(unit_dot(u, ray->dir)) + pow2(unit_dot(v, ray->dir));
    float b = 2 * (unit_dot(calc(ray->org, '-', cylin.center), u) * unit_dot(u, ray->dir) + unit_dot(calc(ray->org, '-', cylin.center), v) * unit_dot(v, ray->dir));
    float c = pow2(unit_dot(calc(ray->org, '-', cylin.center), u)) + pow2(unit_dot(calc(ray->org, '-', cylin.center), v)) - pow2(cylin.radius);

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
        float cal = fabsf(unit_dot(cylin.normal, calc(point_at(ray, x), '-', cylin.center)));
        if (cal > cylin.height / 2)
            x = -1.0;
    }

    t_vec3 c1 = calc(cylin.center, '+', scale(cylin.normal, '*', cylin.height / 2));
    float d1 = hit_plan(ray, (t_obj){.normal = cylin.normal, .d = -unit_dot(c1, cylin.normal)}, min, max);
    if (d1 > 0)
    {
        t_vec3 p1 = point_at(ray, d1);
        if (length(calc(p1, '-', c1)) <= cylin.radius && (x < 0 || d1 < x))
            x = d1;
    }

    t_vec3 c2 = calc(cylin.center, '-', scale(cylin.normal, '*', cylin.height / 2));
    float d2 = hit_plan(ray, (t_obj){.normal = scale(cylin.normal, '*', -1), .d = unit_dot(c2, scale(cylin.normal, '*', -1))}, min, max);
    if (d2 > 0)
    {
        t_vec3 p1 = point_at(ray, d2);
        if (length(calc(p1, '-', c2)) <= cylin.radius && (x < 0 || d2 < x))
            x = d2;
    }
    if (x < min || x > max)
        return -1;
    return x;
}

float hit_cone(t_ray *ray, t_obj cone, float min, float max)
{
    t_vec3 ran = (t_vec3){0, 1, 0};
    if (fabsf(unit_dot(ran, cone.normal)) >= 0.9f)
        ran = (t_vec3){0, 0, 1};
    t_vec3 u = unit_vector(cross(ran, cone.normal));
    t_vec3 v = unit_vector(cross(cone.normal, u));

    float w = pow2(cone.radius) / pow2(cone.height);

    t_vec3 org = calc(ray->org, '-', cone.center);

    float a = pow2(unit_dot(ray->dir, u)) + pow2(unit_dot(ray->dir, v)) - w * pow2(unit_dot(ray->dir, cone.normal));
    float b = 2 * unit_dot(org, u) * unit_dot(ray->dir, u) + 2 * unit_dot(org, v) * unit_dot(ray->dir, v) - (w * 2 * unit_dot(org, cone.normal) * unit_dot(ray->dir, cone.normal) - (2 * pow2(cone.radius) / cone.height) * unit_dot(ray->dir, cone.normal));
    float c = pow2(unit_dot(org, u)) + pow2(unit_dot(org, v)) - pow2(cone.radius) - (w * pow2(unit_dot(org, cone.normal)) - (2 * pow2(cone.radius) / cone.height) * unit_dot(org, cone.normal));

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
    float d1 = unit_dot(cone.normal, calc(point_at(ray, x1), '-', cone.center));
    float d2 = unit_dot(cone.normal, calc(point_at(ray, x2), '-', cone.center));
    if (x1 > 0 && d1 >= 0 && d1 < cone.height)
        x = x1;
    if (x2 > 0 && d2 >= 0 && d2 < cone.height && (x < 0 || x2 < x))
        x = x2;
    if (x < min || x > max)
        return -1;
    return x;
}

// t_vec3 light = {10, 10, 10};
t_color ray_color(t_win *win, t_ray ray)
{
    t_scene *scene = &win->scene;
    t_color color = {.x = 1, .y = 0, .z = 0};
    t_vec3 normal;
    t_vec3 light_dir;

    float closest = FLT_MAX;
    int hit_index = -1;
    float x = 0.0;
    for (int i = 0; i < scene->pos; i++)
    {
        if (scene->objects[i].type == plan_)
            x = hit_plan(&ray, scene->objects[i], ZERO, closest);
        if (scene->objects[i].type == sphere_)
            x = hit_sphere(&ray, scene->objects[i], ZERO, closest);
        else if (scene->objects[i].type == cylinder_)
            x = hit_cylinder(&ray, scene->objects[i], ZERO, closest);
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
        if (scene->objects[hit_index].type == sphere_ || scene->objects[hit_index].type == cone_)
            normal = unit_vector(calc(point_at(&ray, closest), '-', scene->objects[hit_index].center));
        else if (scene->objects[hit_index].type == plan_)
            normal = scene->objects[hit_index].normal;
        else
        {
            printf("Error in ray color\n");
            exit(1);
        }
        light_dir = unit_vector(calc(win->scene.light, '-', point_at(&ray, closest)));
        float d = fmax(unit_dot(normal, light_dir), 0);
        color = calc(color, '+', scale((t_vec3){1, 1, 1}, '*', d));
        color = calc(color, '*', scene->objects[hit_index].color);
    }
    return color;
}

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
    win->scene.objects = ft_calloc(100, sizeof(t_obj));
    win->scene.onj_len = 100;
    win->scene.camera = (t_vec3){.x = 0, .y = 0, .z = FOCAL_LEN};
    win->scene.cam_dir = (t_vec3){.x = 0, .y = 0, .z = -FOCAL_LEN};
    win->scene.w = scale(unit_vector(win->scene.cam_dir), '*', -1);
    win->scene.up = (t_vec3){.x = 0, .y = 1, .z = 0};
    win->scene.screen_height = 2 * tan(win->scene.fov) * FOCAL_LEN;
    win->scene.screen_width = win->scene.screen_height * ((float)win->width / win->height);
    win->scene.u = unit_vector(cross(win->scene.up, win->scene.w));
    win->scene.v = unit_vector(cross(win->scene.w, win->scene.u));
    win->scene.screen_u = scale(win->scene.u, '*', win->scene.screen_width);
    win->scene.screen_v = scale(win->scene.v, '*', -win->scene.screen_height);
    win->scene.pixel_u = scale(win->scene.screen_u, '/', win->width);
    win->scene.pixel_v = scale(win->scene.screen_v, '/', win->height);
    win->scene.center = calc(win->scene.camera, '+', scale(win->scene.w, '*', -FOCAL_LEN));
    win->scene.upper_left = calc(win->scene.center, '-',
                                 scale(calc(win->scene.screen_u, '+', win->scene.screen_v), '/', 2));
    win->scene.first_pixel = calc(win->scene.upper_left, '+',
                                  scale(calc(win->scene.pixel_u, '+', win->scene.pixel_v), '/', 2));
}

int main(void)
{
    t_win *win;
    
    win = new_window(HEIGHT, WIDTH);
    init_scene(win);
    add_objects(win);
    set_pixels(win);
    mlx_hook(win->win, 2, 1L << 0, listen_on_key, win);
    mlx_loop_hook(win->mlx, draw, win);
    mlx_loop(win->mlx);
}
