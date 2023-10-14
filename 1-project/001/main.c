#include "utils.c"
#include "mlx.c"
int WIDTH;
int HEIGHT;

Sphere new_sphere(Coor center, float radius, Col color, Mat type)
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

// bool hit_plan(Plan plan, Ray ray)
// {
//     Coor point = plan.point;
//     Coor height = plan.vector1;
//     Coor width = plan.vector2;
//     for(float i = 0.0 ; i < 1; i += 0.1 )
//     {
//         for(float j = 0.0 ; j < 1; j += 0.1 )
//         {
            
//         }   
//     }
// }

Col ray_color(Win *win, Ray ray, int depth)
{
    if (depth == 50)
        return (Col){0, 0, 0};
    // Plan
    Plan plan = win->plan;


#if 1    
    // Sphere
    float closest = FLT_MAX;
    int hit_index = -1;

    for (int i = 0; i < win->pos; i++)
    {
        float x = hit_sphere(win->spheres[i], ray, 0.0001f, closest);
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
        Ray nray;
        if (sphere.type == Reflectif_)
        {
            float val;
            Coor ray_dir = unit_vector(ray.dir);
            val = -2 * dot(ray_dir, cp_norm);
            ndir = (Coor){.x = ray.dir.x + val * cp_norm.x, .y = ray.dir.y + val * cp_norm.y, .z = ray.dir.z + val * cp_norm.z};
        }
        if (sphere.type == Refractif_)
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
        if (sphere.type == Absorb_)
        {
            ndir = add_(cp_norm, ranv);
        }
        nray = new_ray(p, ndir);
        Col color = ray_color(win, nray, depth + 1);
        color = mul(color, sphere.color);
        return color;
    }
#endif
    float a = 0.5 * (unit_vector(ray.dir).y + 1.0); // value in [0,1]
    float r = (1.0 - a) + a * 0.3;
    float g = (1.0 - a) + a * 0.7;
    float b = (1.0 - a) + a * 1.0;
    return coor(r, g, b);
}

int draw(void *ptr)
{
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    Win *win = (Win *)ptr;
    static Col *sum;
    static int frame;

    frame++;
    if (sum == NULL)
        sum = calloc(WIDTH * HEIGHT, sizeof(Col));
#pragma omp parallel for
    for (int h = 0; h < HEIGHT; h++)
    {
        for (int w = 0; w < WIDTH; w++)
        {
            Coor pixel_center = add_(add_(win->first_pixel, mul_(w + random_float(0, 1), win->pixel_u)), mul_(h + random_float(0, 1), win->pixel_v));
            Coor dir = sub_(pixel_center, win->camera);
            Ray ray = (Ray){.org = win->camera, .dir = dir};
            Col pixel = ray_color(win, ray, 0);
            sum[h * WIDTH + w] = add_(sum[h * WIDTH + w], pixel);
            Col col = div_(sum[h * WIDTH + w], (float)frame);
            if (col.x > 1)
                col.x = 1;
            if (col.y > 1)
                col.y = 1;
            if (col.z > 1)
                col.z = 1;
            int color = ((int)(255.999 * sqrtf(col.r)) << 16) | ((int)(255.999 * sqrtf(col.g)) << 8) | ((int)(255.999 * sqrtf(col.b)));
            char *dst;
            dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel / 8));
            *(unsigned int *)dst = color;
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
    Win var = {0};
    float aspect_ratio = 16.0 / 9.0;
    WIDTH = 800;
    HEIGHT = (int)((float)WIDTH / aspect_ratio);
    if (HEIGHT < 1)
        HEIGHT = 1;

    float vfov = 20;
    Coor lookfrom = coor(-2, 3, 20); // Point camera is looking from
    Coor lookat = coor(0, 0, -1);    // Point camera is looking at
    Coor vup = coor(0, 1, 0);        // Camera-relative "up" direction

    var.camera = lookfrom; //(Coor){.x = 0, .y = 0, .z = 0};
    float focal_length = length(sub_(lookfrom, lookat));
    double theta = degrees_to_radians(vfov);
    float h = tan(theta / 2);
    float screen_height = 2 * h * focal_length;
    float screen_width = screen_height * ((double)(WIDTH) / HEIGHT);

    var.w = unit_vector(sub_(lookfrom, lookat));
    var.u = unit_vector(cross_(vup, var.w));
    var.v = cross_(var.w, var.u);

    var.screen_u = mul_(screen_width, var.u);        //(Coor){.x = screen_width, .y = 0, .z = 0};
    var.screen_v = mul_(screen_height, neg_(var.v)); //(Coor){.x = 0, .y = -screen_height, .z = 0};

    var.pixel_u = div_(var.screen_u, WIDTH);
    var.pixel_v = div_(var.screen_v, HEIGHT);

    // viewport_upper_left = center - (focal_length * w) - viewport_u/2 - viewport_v/2;
    Coor viewport_center = sub_(var.camera, mul_(focal_length, var.w));
    Coor upper_left = sub_(sub_(viewport_center, div_(var.screen_u, 2)), div_(var.screen_v, 2));
    var.first_pixel = add_(upper_left, mul_(0.5, add_(var.pixel_u, var.pixel_v)));

    var.pos = 0;

#if 1
    var.spheres[var.pos++] = new_sphere(coor(0.0, -100.5, -1.0), 100, coor(0.8, 0.8, 0.0), Absorb_);
    var.spheres[var.pos++] = new_sphere(coor(0.0, 0.0, -1.0), 0.5, coor(0.7, 0.3, 0.3), Absorb_);     // center
    var.spheres[var.pos++] = new_sphere(coor(-1.5, 0.0, -1.0), 0.5, coor(0.8, 0.8, 0.8), Reflectif_); // left
    var.spheres[var.pos++] = new_sphere(coor(1.5, 0.0, -1.0), 0.5, coor(0.8, 0.6, 0.2), Refractif_);  // right
#endif
    var.plan.point = coor(0, 0, 0);
    var.plan.vector1 = coor(0, 5, 0);
    var.plan.vector2 = coor(5, 0, 0);

    var.mlx = mlx_init();
    var.win = mlx_new_window(var.mlx, WIDTH, HEIGHT, "Hello world!");
    var.img = mlx_new_image(var.mlx, WIDTH, HEIGHT);
    var.addr = mlx_get_data_addr(var.img, &var.bits_per_pixel, &var.line_length, &var.endian);

    mlx_loop_hook(var.mlx, draw, &var);
    mlx_hook(var.win, 2, 1L << 0, listen, &var);
    mlx_hook(var.win, 17, 1L << 0, close_window, &var);
    mlx_loop(var.mlx);
}