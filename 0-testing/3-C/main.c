#include "coor.c"
#include "mlx.c"
int WIDTH;
int HEIGHT;

Sphere new_sphere(Coor center, float radius, Color color, Mat type)
{
    Sphere new;
    new.center = center;
    new.radius = radius;
    new.color = color;
    new.type = type;
    return new;
}

float hit_sphere(Sphere sphere, Ray ray, float min, float max)
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
    if (sol < min || sol > max)
        sol = (-half_b + sq_delta) / a;
    if (sol < min || sol > max)
        return -1.0;
    return (sol);
}

Color ray_color(Win *win, Ray ray, int depth)
{
    if (depth == 5)
        return (Color){};

    float closest = FLT_MAX;
    int hit_index = -1;

    for (int i = 0; i < win->pos; i++)
    {
        // printf("did hit\n");
        float x = hit_sphere(win->spheres[i], ray, 0.0001, closest);
        if (x > 0.0 && x < closest)
        {
            hit_index = i;
            closest = x;
        }
    }
    if (hit_index != -1)
    {
        Sphere sphere = win->spheres[hit_index];

        // point coordinates
        Coor p = point_at(ray, closest);
        Coor cp_norm = unit_vector(sub_(p, sphere.center));
        bool same_dir = dot(cp_norm, ray.dir) >= 0;
        if (same_dir) // to be used when drawing triangle
            cp_norm = (Coor){.x = -cp_norm.x, .y = -cp_norm.y, .z = -cp_norm.z};

        Coor ranv = random_unit_vector();
        Coor ndir;
        if (sphere.type == Reflectif_)
        {
            float val;
            Coor ray_dir = unit_vector(ray.dir);
            val = -2 * dot(ray_dir, cp_norm);
            ndir = (Coor){.x = ray.dir.x + val * cp_norm.x, .y = ray.dir.y + val * cp_norm.y, .z = ray.dir.z + val * cp_norm.z};
        }
        else
            ndir = add_(cp_norm, ranv);

        Ray nray = new_ray(p, ndir);
        Color color = ray_color(win, nray, depth + 1);
        color = mul(color, sphere.color);
        return color;
    }

    float a = 0.5 * (unit_vector(ray.dir).y + 1.0); // value in [0,1]
    float r = (1.0 - a) + a * 0.3;
    float g = (1.0 - a) + a * 0.7;
    float b = (1.0 - a) + a * 1.0;
    return new_coor(r, g, b);
}

void write_color(Win *win, int x, int y, Color col)
{
    int color = ((int)(255.999 * sqrt(col.r)) << 16) | ((int)(255.999 * sqrt(col.g)) << 8) | ((int)(255.999 * sqrt(col.b)));

    char *dst;
    dst = win->addr + (y * win->line_length + x * (win->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

int draw(void *ptr)
{
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    Win *win = (Win *)ptr;
    static Color *sum;
    static int frame;

    frame++;
    if (sum == NULL)
        sum = calloc(WIDTH * HEIGHT, sizeof(Color));
    for (int h = 0; h < HEIGHT; h++)
    {
        for (int w = 0; w < WIDTH; w++)
        {
            Coor pixel_center = add_(add_(win->first_pixel, mul_(w + random_float(0, 1), win->pixel_u)), mul_(h + random_float(0, 1), win->pixel_v));
            Coor dir = sub_(pixel_center, win->camera);
            Ray ray = (Ray){.org = win->camera, .dir = dir};
            Color pixel = ray_color(win, ray, 0);
            sum[h * WIDTH + w] = add_(sum[h * WIDTH + w], pixel);

            Color col = div_(sum[h * WIDTH + w], (float)frame);

            int color = ((int)(255.999 * sqrtf(col.r)) << 16) | ((int)(255.999 * sqrtf(col.g)) << 8) | ((int)(255.999 * sqrtf(col.b)));
            char *dst;
            dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel / 8));
            *(unsigned int *)dst = color;
        }
    }
    mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    double frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
    printf("%.2f: render frame %d\n",frame_time, frame);            
    return 0;
}

int main(void)
{
    Win var;
    float focal_length = 1;
    float aspect_ratio = 1; // 16.0 / 9.0;
    WIDTH = 512;
    HEIGHT = (int)((float)WIDTH / aspect_ratio);

    float viewport_height = 2.0;
    float viewport_width = viewport_height * ((float)WIDTH / HEIGHT);

    var.viewport_u = (Coor){.x = viewport_width, .y = 0, .z = 0};
    var.viewport_v = (Coor){.x = 0, .y = -viewport_height, .z = 0};
    var.camera = (Coor){.x = 0, .y = 0, .z = 0};

    var.pixel_u = div_(var.viewport_u, WIDTH);
    var.pixel_v = div_(var.viewport_v, HEIGHT);

    Coor viewport_center = sub_(var.camera, (Coor){.x = 0, .y = 0, .z = focal_length});
    Coor upper_left = sub_(sub_(viewport_center, div_(var.viewport_u, 2)), div_(var.viewport_v, 2));
    var.first_pixel = add_(upper_left, mul_(0.5, add_(var.pixel_u, var.pixel_v)));

    /*
        auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8));
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2))

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    */

    var.pos = 0;
    var.spheres[var.pos++] = new_sphere(new_coor(0.0, -100.5, -1.5), 100, new_coor(0.8, 0.8, 0.0), Absorb_);
    var.spheres[var.pos++] = new_sphere(new_coor(0.0, 0.0, -1.5), 0.5, new_coor(0.7, 0.3, 0.3), Absorb_);
    var.spheres[var.pos++] = new_sphere(new_coor(-1.0, 0.0, -1.5), 0.5, new_coor(0.8, 0.8, 0.8), Reflectif_);
    var.spheres[var.pos++] = new_sphere(new_coor(1.0, 0.0, -1.5), 0.5, new_coor(0.8, 0.6, 0.2), Reflectif_);

    var.mlx = mlx_init();
    var.win = mlx_new_window(var.mlx, WIDTH, HEIGHT, "Hello world!");
    var.img = mlx_new_image(var.mlx, WIDTH, HEIGHT);
    var.addr = mlx_get_data_addr(var.img, &var.bits_per_pixel, &var.line_length, &var.endian);

    mlx_loop_hook(var.mlx, draw, &var);
    mlx_hook(var.win, 2, 1L << 0, listen, &var);
    mlx_hook(var.win, 17, 1L << 0, close_window, &var);
    mlx_loop(var.mlx);
}