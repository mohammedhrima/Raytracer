#include "utils.c"

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
    float t = plan.d - dot(plan.normal, ray.org);
    float div = dot(ray.dir, plan.normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
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
Ray render_object(Obj obj, Ray ray, float closest)
{
    // point coordinates
    Vec3 p = point_at(ray, closest);
    Vec3 cp_norm;

    if (obj.type == sphere_)
        cp_norm = unit_vector(sub_vec3(p, obj.center));
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

        float cos_theta = dot(ray.dir, cp_norm) / (length(ray.dir) * length(cp_norm)); // fmin(dot(-unit_dir, normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
#if 0
        if (refraction_ratio * sin_theta > 1.0 || reflectance(cos_theta, refraction_ratio) > random_float(-FLT_MAX, FLT_MAX))
#else
        if (refraction_ratio * sin_theta > 1.0)
#endif
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
    else if (obj.mat == Abs_)
        ndir = add_vec3(cp_norm, ranv);
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
        float x = .0;
        for (int i = 0; i < scene->pos; i++)
        {
            if (scene->objects[i].type == sphere_)
                x = hit_sphere(scene->objects[i], ray, ZERO, closest);
            else if (scene->objects[i].type == plan_)
                x = hit_plan(scene->objects[i], ray, ZERO, closest);
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
            light = add_vec3(light, mul_vec3_(attenuation, mul_vec3(obj->light_intensity, obj->light_color)));
            attenuation = mul_vec3_(attenuation, obj->color);
        }
        else
        {
            float a = 0.5 * (unit_vector(ray.dir).y + 1.0);
            float r = (1.0 - a) + a * .3;
            float g = (1.0 - a) + a * .7;
            float b = (1.0 - a) + a * 1.0;

            light = add_vec3(light, mul_vec3_(attenuation, (Color){r, g, b}));
            break;
        }
        if (attenuation.x <= ZERO && attenuation.y <= ZERO && attenuation.z <= ZERO)
            break;
    }
    return light;
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
            Color pixel = ray_color(win, ray, 5);
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

    scene->view_angle = degrees_to_radians(60);
    scene->camera = (Vec3){0, 0, -1};
    scene->camera_dir = sub_vec3((Vec3){0, 0, 0}, scene->camera); // direction
    Vec3 rightv = (Vec3){1, 0, 0};                                // used for getting u,v,w

    scene->len = 2; // length(sub_vec3(scene->camera, scene->camera_dir)); // distance from screen
    float tang = tan(scene->view_angle / 2);
    float screen_height = 2 * tang * scene->len;
    float screen_width = screen_height * ((float)win.width / win.height);

    scene->w = unit_vector(div_vec3(scene->camera_dir, scene->len)); // unit_vector(sub_vec3(scene->camera_dir, scene->camera)); // lookat - camera  z+ (get w vector)
    scene->v = unit_vector(cross_(rightv, scene->w));                // y+ (get v vector)
    scene->u = unit_vector(cross_(scene->w, scene->v));              // x+ (get u vector)

    // viewport steps
    scene->screen_u = mul_vec3(screen_width, scene->u);
    scene->screen_v = mul_vec3(screen_height, scene->v);
    // window steps
    scene->pixel_u = div_vec3(scene->screen_u, win.width);
    scene->pixel_v = div_vec3(scene->screen_v, win.height);

    Vec3 screen_center = add_vec3(scene->camera, mul_vec3(scene->len, scene->w));                       // camera + len * w
    Vec3 upper_left = sub_vec3(screen_center, div_vec3(add_vec3(scene->screen_u, scene->screen_v), 2)); // center - screen_u / 2 - screen_v / 2 = center - (screen_u+screen_v)/2
    scene->first_pixel = add_vec3(upper_left, div_vec3(add_vec3(scene->pixel_u, scene->pixel_v), 2)); // upper_left + (pixel_u + pixel_v) / 2

    // add objects
#if 1
    Vec3 colors[] = {
        // {1, 1, 1},
        {0.92, 0.19, 0.15},
        {0.42, 0.92, 0.72},
        {0.42, 0.87, 0.92},
        {0.30, 0.92, 0.64},
        {0.39, 0.92, 0.63},
        {0.42, 0.92, 0.80},
        {0.47, 0.16, 0.92},
        {0.42, 0.58, 0.92},
        {0.92, 0.40, 0.30},
        {0.61, 0.75, 0.24},
        {0.83, 0.30, 0.92},
        {0.23, 0.92, 0.08},
    };
    struct
    {
        Vec3 normal;
        float dist; // ditance from camera
        Mat mat;
    } plans[] = {
        {(Vec3){0, -1, 0}, 4, Abs_}, // up
        {(Vec3){0, 1, 0}, 4, Abs_},  // down
        {(Vec3){0, 0, 1}, 12, Abs_}, // behind
        {(Vec3){1, 0, 0}, 4, Abs_},  // right
        {(Vec3){-1, 0, 0}, 4, Abs_}, // left
        {(Vec3){}, 0, 0},
    };
    struct
    {
        Vec3 org;
        float rad;
        Mat mat;
    } spheres[] = {
        {(Vec3){0, 0, 12.0}, 2, Abs_}, // center
        {(Vec3){}, 0, 0},
        {(Vec3){0, 4.0, 1.5}, 2, Refl_},    // up
        {(Vec3){-1.5, .5, 1.0}, .5, Refl_}, // left
        {(Vec3){1.5, .5, 1.0}, .5, Refl_},  // right
    };

    int i = 0;
    while (spheres[i].mat)
    {
        Vec3 org = spheres[i].org;
        float rad = spheres[i].rad;
        Mat mat = spheres[i].mat;
        scene->objects[scene->pos] = new_sphere(org, rad, colors[scene->pos % (sizeof(colors) / sizeof(*colors))], mat);
        i++;
        scene->pos++;
    }
    i = 0;
    while (plans[i].mat)
    {
        Vec3 normal = plans[i].normal;
        float dist = plans[i].dist;
        Mat mat = plans[i].mat;
        scene->objects[scene->pos] = new_plan(normal, dist, colors[scene->pos % (sizeof(colors) / sizeof(*colors))], mat);
        i++;
        scene->pos++;
    }
    scene->objects[0].light_intensity = 1;
    scene->objects[0].light_color = (Color){1, 1, 1};
#else
    Vec3 p1, p2, p3;
    p1 = (Vec3){1, 1, 2};
    p2 = (Vec3){2, 1, 2};
    p3 = (Vec3){1.5, 2, 2};
    scene->objects[scene->pos++] = new_triangle(p1, p2, p3, colors[1], Abs_);
#endif

    win.mlx = mlx_init();
    win.win = mlx_new_window(win.mlx, win.width, win.height, "Mini Raytracer");
    win.img = mlx_new_image(win.mlx, win.width, win.height);
    win.addr = mlx_get_data_addr(win.img, &win.bits_per_pixel, &win.line_length, &win.endian);

    mlx_loop_hook(win.mlx, draw, &win);
    mlx_key_hook(win.win, listen, &win);
    mlx_loop(win.mlx);
}
