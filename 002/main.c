#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

typedef struct
{
    double x;
    double y;
    double z;
} Coor;
typedef Coor Color;

#define new(xv, yv, zv) \
    (Coor) { .x = (double)xv, .y = (double)yv, .z = (double)zv }

#define pow2(n) n *n

#define sub(leftv, rightv) \
    new (leftv.x - rightv.x, leftv.y - rightv.y, leftv.z - rightv.z)

#define add(leftv, rightv) \
    new (leftv.x + rightv.x, leftv.y + rightv.y, leftv.z + rightv.z)

#define mul(leftv, rightv) \
    new (leftv.x * rightv.x, leftv.y * rightv.y, leftv.z * rightv.z)

#define RGB(r, g, b) new ((double)r / 255.999, (double)g / 255.999, (double)b / 255.999)

double vector_len_squared(Coor v)
{
    return pow2(v.x) + pow2(v.y) + pow2(v.z);
}

double vector_len(Coor v)
{
    return sqrt(vector_len_squared(v));
}

Coor unit_vector(Coor v)
{
    double len = vector_len(v);
    if (len == 0)
        return new (0.0, 0.0, 0.0);
    return new (v.x / len, v.y / len, v.z / len);
}

double dot(Coor v, Coor u)
{
    return v.x * u.x + v.y * u.y + v.z * u.z;
}

double random_double(double min, double max)
{
    double n = rand() / (RAND_MAX + 1.0); // random double [0,1]
    return min + (max - min) * n;
}

Coor random_vector(double min, double max)
{
    return new (random_double(min, max), random_double(min, max), random_double(min, max));
}

Coor random_unit()
{
    while (1)
    {
        Coor v = random_vector(-1.0, 1.0);
        if (vector_len(v) < 1)
            return v;
    }
}

Coor random_unit_vector()
{
    return unit_vector(random_unit());
}

typedef struct
{
    Coor org;
    Coor dir;
} Ray;
typedef enum
{
    Reflec,
    Absorb
} Mater;
typedef struct
{
    Coor center;
    double radius;
    Color color;
    Mater mat;
} Sphere;

Sphere new_sphere(Coor center, double radius, Color color, Mater mat)
{
    Sphere s;
    s.center = center;
    s.radius = radius;
    s.color = color;
    s.mat = mat;
    return s;
}

Ray new_ray(Coor org, Coor dir)
{
    return (Ray){.org = org, .dir = dir};
}

Coor point_at(Ray ray, double t)
{
    return new (ray.org.x + t * ray.dir.x, ray.org.y + t * ray.dir.y, ray.org.z + t * ray.dir.z);
}

double hit_sphere(Sphere sphere, Ray ray, double min, double max)
{
    Coor OC = sub(ray.org, sphere.center);
    double a = vector_len_squared(ray.dir);
    double half_b = dot(OC, ray.dir);
    double c = vector_len_squared(OC) - sphere.radius * sphere.radius;
    double delta = half_b * half_b - a * c;
    if (delta < 0.0)
        return -1.0;
    double sol = (-half_b - sqrt(delta)) / a;
    if (sol < min || sol > max)
        sol = (-half_b + sqrt(delta)) / a;
    if (sol < min || sol > max)
        return -1.0;
    return (sol);
}

Color ray_color(Ray ray, int depth)
{
    if (depth == 50)
        return new (0, 0, 0);

    Sphere sphere1 = new_sphere(new (-1, 0, 2), 0.5, RGB(48, 134, 111), Reflec);
    Sphere sphere2 = new_sphere(new (1, 0, 2), 0.5, RGB(108, 134, 48), Reflec);
    Sphere sphere3 = new_sphere(new (0, 0, 2), 0.5, RGB(60, 33, 7), Reflec);
    Sphere sphere4 = new_sphere(new (0, -100.5, 1), 100, RGB(255.999, 0, 0), Absorb);
    Sphere sphere;

    Sphere spheres[] = {sphere1, sphere2, sphere3, sphere4};
    int len = sizeof(spheres) / sizeof(spheres[0]);

    double closest = DBL_MAX;
    int hit_index = -1;

    for (int i = 0; i < len; i++)
    {
        double t = hit_sphere(spheres[i], ray, 0.0001, closest);
        if (t > 0.0 && t < closest)
        {
            hit_index = i;
            closest = t;
        }
    }

    // double sol = hit_sphere(sphere, ray, 0.001, closest);
    if (hit_index != -1.0)
    {
        sphere = spheres[hit_index];
        // point coordinates
        Coor p = point_at(ray, closest);
        Coor cp_norm = unit_vector(sub(p, sphere.center));
        bool same_dir = dot(cp_norm, ray.dir) >= 0;
        if (same_dir) // to be used when drawing triangle
            cp_norm = new (-cp_norm.x, -cp_norm.y, -cp_norm.z);

        // new direction
        // it should be unit vector because it has coordinates in [-1,1]
        Coor ranv = random_unit_vector();
        Coor ndir;
        if (sphere.mat == Reflec)
        {

            double val = 1;
            // reflectif
            // get the unit vector
            Coor ray_dir = unit_vector(ray.dir);
            // dot product will return negative value
            // because they look at different direction
            // -2 to make it even longer at the reverse direction
            val = -2 * dot(ray_dir, cp_norm);
            ndir = new (ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z);
        }
        else
            ndir = add(cp_norm, ranv);
        Ray nray = new_ray(p, ndir);
        // assert(0);
        Color color = ray_color(nray, depth + 1);
        color = mul(color, sphere.color);
        return color;
    }

    Coor unit = unit_vector(ray.dir);
    double a = 0.5 * (unit.y + 1.0); // value in [0,1]
    double r = (1.0 - a) + a * 0.5;
    double g = (1.0 - a) + a * 0.7;
    double b = (1.0 - a) + a * 1.0;
    return new (r, g, b);
}
void write_color(Color color)
{
    printf("%d %d %d ", (int)(255.999 * sqrt(color.x)), (int)(255.999 * sqrt(color.y)), (int)(255.999 * sqrt(color.z)));
}

int main()
{
    double aspect_ratio = 16.0 / 9.0;
    int width = 800;
    int height = (int)(width / aspect_ratio);
    if (height < 1)
        height = 1;

    double focal_len = 1.0;
    double screen_width = 2.0;
    double screen_height = screen_width / aspect_ratio;

    Coor camera = new (0, 0, 0);

    // Coor screen_u = new (screen_width, 0, 0);
    // Coor screen_v = new (0, -screen_height, 0);
    // Coor screen_w = new (0, 0, focal_len);

    // Coor pixel_u = new (screen_u.x / width, 0, 0);
    // Coor pixel_v = new (0, screen_v.y / height, 0);
    // Coor pixel_w = new (0, 0, screen_w.z);
    double y_step = -screen_height / height; // - because we are going down
    double x_step = screen_width / width;

    double x, y, z;
    x = camera.x - screen_width / 2;
    y = camera.y + screen_height / 2;
    z = camera.z + focal_len;
    Coor stpixel = new (x, y, z);

    printf("P3\n%d %d\n255\n", width, height);
    for (int h = 0; h < height; h++)
    {
        dprintf(2, "\rScanlines remaining: %d\n", (height - h));
        for (int w = 0; w < width; w++)
        {
            int rays_per_pixel = 32;
            Color final = new (0, 0, 0);
            for (int i = 0; i < rays_per_pixel; i++)
            {
                double a = random_double(-0.5, 0.5);
                double b = random_double(-0.5, 0.5);
                Coor point = new (stpixel.x + (w + a) * x_step, stpixel.y + (h + b) * y_step, stpixel.z);
                Coor dir = sub(point, camera);
                Ray ray = new_ray(camera, dir);
                Color color = ray_color(ray, 0);
                final = add(final, color);
            }
            final = new (final.x / rays_per_pixel, final.y / rays_per_pixel, final.z / rays_per_pixel);
            write_color(final);
        }
        printf("\n");
    }
}