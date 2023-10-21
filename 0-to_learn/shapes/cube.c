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
#include <string.h>

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
} Win;


typedef union 
{
    struct {float x, y, z;};
    struct {float r, g, b;};
} v3;

typedef struct
{
    float x, y;
} v2;

v3 V3(float x, float y, float z)
{
    return (v3){x, y, z};
}

v3 v3_add(v3 a, v3 b)
{
    return (v3){a.x + b.x, a.y + b.y, a.z + b.z};
}

v3 v3_sub(v3 a, v3 b)
{
    return (v3){a.x - b.x, a.y - b.y, a.z - b.z};
}

v3 v3_scale(v3 a, float b)
{
    return (v3){a.x * b, a.y * b, a.z * b};
}

float v3_dot(v3 a, v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float v3_length(v3 a)
{
    return sqrtf(v3_dot(a, a));
}

v3 v3_noz(v3 a)
{
    float len = v3_length(a);

    if (len <= 0.00001f)
        return V3(0, 0, 0);
    return v3_scale(a, 1.f / len);
}

v3 v3_cross(v3 a, v3 b)
{
    return (v3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

v2 V2(float x, float y)
{
    return (v2){x, y};
}

v2 v2_add(v2 a, v2 b)
{
    return (v2){a.x + b.x, a.y + b.y};
}

v2 v2_sub(v2 a, v2 b)
{
    return (v2){a.x - b.x, a.y - b.y};
}

#define PI 3.1415926535897932385

#define DEG_TO_RAD (PI / 180.0)

Win *window;
v3 camera;
float focal_length;
float fov;
float width, height;
int first_frame = 1;
float *zbuffer;

v2 project_3d_to_2d(v3 p)
{

    v2 res;

    res.x = -(focal_length * p.x) / p.z;
    res.y = -(focal_length * p.y) / p.z;

    res.x = ((res.x / (width / 2)) + 1) * 0.5f * window->width;
    res.y = ((res.y / (height / 2)) + 1) * 0.5f * window->height;
    return res;
}

v3 get_barycentic_coordinates(v2 p0, v2 p1, v2 p2, v2 p)
{
    p = v2_sub(p, p0);

    v2 u = v2_sub(p1, p0);
    v2 v = v2_sub(p2, p0);

    float det = u.x * v.y - v.x * u.y;

    if (fabsf(det) < 0.0001f)
        return (v3){-1, -1, -1};
    det = 1.0f / det;

    float alpha = p.x * v.y * det + p.y * (-v.x) * det;
    float beta = p.x * (-u.y) * det + p.y * u.x * det;

    if (alpha < 0 || beta < 0 || alpha > 1 || beta > 1 || alpha + beta > 1)
        return (v3){-1, -1, -1};
    return (v3){alpha, beta, 1 - (alpha + beta)}; 
}

void draw_triangle(Win *win, v3 tp0, v3 tp1, v3 tp2, v3 color)
{
    v2 p0 = project_3d_to_2d(tp0);
    v2 p1 = project_3d_to_2d(tp1);
    v2 p2 = project_3d_to_2d(tp2);

    v2 box_min = {fmin(p0.x, fmin(p1.x, p2.x)), fmin(p0.y, fmin(p1.y, p2.y))};
    v2 box_max = {fmax(p0.x, fmax(p1.x, p2.x)), fmax(p0.y, fmax(p1.y, p2.y))};

    int min_x = floorf(box_min.x);
    int max_x = ceilf(box_max.x);
    int min_y = floorf(box_min.y);
    int max_y = ceilf(box_max.y);

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x > win->width) max_x = win->width;
    if (max_y > win->height) max_y = win->height;

    

    for (int y = min_y; y < max_y; y++)
    {
        for (int x = min_x; x < max_x; x++)
        {
            v3 b = get_barycentic_coordinates(p0, p1, p2, (v2){x, y});
            
            v3 p = v3_add(v3_scale(tp0, b.z), v3_add(v3_scale(tp1, b.x), v3_scale(tp2, b.y)));

            if (b.x > 0 && p.z > zbuffer[y * win->width + x])
            {
                zbuffer[y * win->width + x] = p.z;

                v3 c0 = {1, 0, 0};
                v3 c1 = {0, 1, 0};
                v3 c2 = {0, 0, 1};
                color = v3_add(v3_scale(c0, b.z), v3_add(v3_scale(c1, b.x), v3_scale(c2, b.y)));

                uint32_t color32 = ((uint32_t)(color.r * 255 + 0.5f) << 16) |
                        ((uint32_t)(color.g * 255 + 0.5f) << 8) |
                        ((uint32_t)(color.b * 255 + 0.5f) << 0);

                uint32_t *pixel = (uint32_t *)win->addr + (win->height - y - 1) * win->width + x;

                *pixel = color32;

            }
        }
    }
}

void draw_cube(Win *win, v3 c, v3 w, float radius, v3 color)
{
    w = v3_noz(w);

    v3 up = {0, 1, 0};

    if (fabsf(v3_dot(up, w)) > 0.9)
        up = V3(1, 0, 0);
    
    v3 u = v3_noz(v3_cross(up, w));
    v3 v = v3_noz(v3_cross(w, u));

    printf("u: %f %f %f\n", u.x, u.y, u.z);
    printf("v: %f %f %f\n", v.x, v.y, v.z);
    printf("w: %f %f %f\n", w.x, w.y, w.z);
    u = v3_scale(u, radius);
    v = v3_scale(v, radius);
    w = v3_scale(w, radius);

    

    v3 p00 = v3_add(c, v3_add(v3_scale(u, -1), v3_add(v3_scale(v, -1), v3_scale(w, -1))));//c - u - v - w;
    v3 p01 = v3_add(c, v3_add(v3_scale(u, +1), v3_add(v3_scale(v, -1), v3_scale(w, -1))));//c + u - v - w;
    v3 p02 = v3_add(c, v3_add(v3_scale(u, +1), v3_add(v3_scale(v, +1), v3_scale(w, -1))));//c + u + v - w;
    v3 p03 = v3_add(c, v3_add(v3_scale(u, -1), v3_add(v3_scale(v, +1), v3_scale(w, -1))));//c - u + v - w;
    v3 p10 = v3_add(c, v3_add(v3_scale(u, -1), v3_add(v3_scale(v, -1), v3_scale(w, +1))));//c - u - v + w;
    v3 p11 = v3_add(c, v3_add(v3_scale(u, +1), v3_add(v3_scale(v, -1), v3_scale(w, +1))));//c + u - v + w;
    v3 p12 = v3_add(c, v3_add(v3_scale(u, +1), v3_add(v3_scale(v, +1), v3_scale(w, +1))));//c + u + v + w;
    v3 p13 = v3_add(c, v3_add(v3_scale(u, -1), v3_add(v3_scale(v, +1), v3_scale(w, +1))));//c - u + v + w;

    v3 triangles[][3] = {
       {p00, p01, p02}, {p00, p02, p03}, // front
      {p10, p11, p12}, {p10, p12, p13}, // back
        {p01, p11, p12}, {p01, p12, p02}, // right
        {p10, p00, p03}, {p10, p03, p13}, // left
        {p03, p02, p12}, {p03, p12, p13}, // up
        {p00, p01, p11}, {p00, p11, p10}, // down
    
    };
    int n = sizeof(triangles) / sizeof(*triangles);
    for (int i = 0; i < n; i++)
    {
        draw_triangle(win, triangles[i][0], triangles[i][1], triangles[i][2], color);
    }

}

int draw(void *ptr)
{
    Win *win = ptr;

    if (first_frame)
    {
        camera = (v3){};
        focal_length = 1;
        fov = 65;

        width = tan((fov*0.5f) * DEG_TO_RAD) * 2 * focal_length;
        height = width * ((float)win->height / win->width);

        first_frame = 0;
    }

    memset(win->addr, 0, win->width * win->height * 4);

    for (int y = 0; y < win->height; y++)
    {
        for (int x = 0; x < win->width; x++)
        {
            zbuffer[y * win->width + x] = -FLT_MAX;
        }
    }

    static float a = 0;
    v3 o = {0, 0, -2};
    v3 v = {0, 1, 0};
    v3 u = {sin(a), cos(a), sin(a) * cos(a)};

    a += 0.01f;

    //draw_triangle(win, o, v3_add(o, u), v3_add(o, v), V3(1, 1, 1));

    v3 axis = {sin(a), cos(a), cos(a)};
    draw_cube(win, V3(0, 0, -3), axis, 0.5, V3(.4, .8, .5));

    mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
    return 0;
}

int main(void)
{
    Win win = {};

    window = &win;
    win.width = 512;
    win.height = 512;
    
    zbuffer = malloc(win.width * win.height * sizeof(float));

    win.mlx = mlx_init();
    win.win = mlx_new_window(win.mlx, win.width, win.height, "Mini Raytracer");
    win.img = mlx_new_image(win.mlx, win.width, win.height);
    win.addr = mlx_get_data_addr(win.img, &win.bits_per_pixel, &win.line_length, &win.endian);

    mlx_loop_hook(win.mlx, draw, &win);
    //mlx_key_hook(win.win, listen, &win);
    mlx_loop(win.mlx);
}