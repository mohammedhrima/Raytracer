#include "00.utils.c"

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

// TODO: check Barycentric Coordinates
float hit_triangle(Obj trian, Ray ray, float min, float max)
{
    float t = dot(trian.normal, sub_vec3(trian.p1, ray.org));
    float div = dot(trian.normal, ray.dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;

    Vec3 p = sub_vec3(point_at(ray, t), trian.p1);

    Vec3 u = sub_vec3(trian.p2, trian.p1);
    Vec3 v = sub_vec3(trian.p3, trian.p1);
    Vec3 normal = cross_(u, v);
    Vec3 w = mul_vec3(1.0 / dot(normal, normal), normal);
    float alpha = dot(cross_(p, v), w);
    float beta = -dot(cross_(p, u), w);

    if (alpha < 0 || beta < 0 || alpha + beta > 1)
        return -1;

    return t;
}

float v = 0;
float va = 0.1f;
float vaa = 0.001f;
float hit_hemisphere(Obj hemisphere, Ray ray, float min, float max)
{
    // hemisphere.color = (Color){hemisphere.color.b * va, hemisphere.color.g * va, hemisphere.color.b * va};
    Vec3 OC = sub_vec3(ray.org, hemisphere.center);
    float a = length_squared(ray.dir);
    float half_b = dot(OC, ray.dir);
    float c = length_squared(OC) - hemisphere.radius * hemisphere.radius;
    float delta = half_b * half_b - a * c;
    if (delta < .0)
        return -1.0;
    float sq_delta = sqrtf(delta);
    float sol = (-half_b - sq_delta) / a;

    Vec3 p = unit_vector(sub_vec3(point_at(ray, sol), hemisphere.center));
    Vec3 axis1 = {-1, -1 - v, 0};
    Vec3 axis2 = {-1, 1 + v, 0};
    // Vec3 axis1 = {-1, -1, 0};
    // Vec3 axis2 = {-1, 1, 0};
    float d1 = dot(p, axis1);
    float d2 = dot(p, axis2);
    if (sol <= min || sol >= max || (d2 < 0 && d1 < 0))
    {
        sol = (-half_b + sq_delta) / a;
        p = unit_vector(sub_vec3(point_at(ray, sol), hemisphere.center));
        d1 = dot(p, axis1);
        d2 = dot(p, axis2);
        if (sol <= min || sol >= max || (d2 < 0 && d1 < 0))
            sol = -1;
    }
    return sol;
}

Ray render_object(Obj obj, Ray ray, float closest)
{
    // point coordinates
    Vec3 cp_norm;
    Vec3 p = point_at(ray, closest);
    if (obj.type == sphere_)
        cp_norm = unit_vector(sub_vec3(p, obj.center));
    if (obj.type == hemisphere_)
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

// mlx
int frame;
Color *sum;

void translate(Scene *scene, Vec3 move)
{
    Vec3 x = mul_vec3(move.x, scene->u);
    Vec3 y = mul_vec3(move.y, scene->v);
    Vec3 z = mul_vec3(move.z, scene->w);

    scene->camera = add_vec3(scene->camera, add_vec3(x, add_vec3(y, z)));
    frame = 0;
    free(sum);
    sum = NULL;
}

Vec3 rotate(float angle, Vec3 u, int axes)
{
    float cos_ = cos(angle);
    float sin_ = sin(angle);
    switch (axes)
    {
    case 'x':
    {
        return (Vec3){
            u.x,
            u.y * cos_ - u.z * sin_,
            u.y * sin_ + u.z * cos_};
    }
    case 'y':
    {
        return (Vec3){
            u.x * cos_ + u.z * sin_,
            u.y,
            -u.x * sin_ + u.z * cos_,
        };
    }
    case 'z':
    {
        return (Vec3){
            u.x * cos_ - u.y * sin_,
            u.x * sin_ + u.y * cos_,
            u.z};
    }
    default:
    {
        printf("Error in rotation\n");
        exit(0);
    }
    }
    return (Vec3){};
}
int curr_x;
int curr_y;
int old_x;
int old_y;
float y_rotation = 0;
float x_rotation = 0;
float angle = 6;
int is_mouse_down = 0;
void init(Win *win);
int listen_on_key(int code, Win *win)
{
    float angle = 10;
    Scene *scene = &win->scene;
    struct
    {
        Vec3 move;
        char *msg;
    } trans[1000] = {
        [FORWARD] = {(Vec3){0, 0, -.5}, "forward"},
        [BACKWARD] = {(Vec3){0, 0, .5}, "backward"},
        [UP] = {(Vec3){0, .5, 0}, "up"},
        [DOWN] = {(Vec3){0, -.5, 0}, "down"},
        [LEFT] = {(Vec3){-.5, 0, 0}, "left"},
        [RIGHT] = {(Vec3){.5, 0, 0}, "right"},
    };

    switch (code)
    {
    case ESC:
        mlx_destroy_window(win->mlx, win->win);
        exit(0);
    case FORWARD:
    case BACKWARD:
    case UP:
    case DOWN:
    case RIGHT:
    case LEFT:
        printf("%s\n", trans[code].msg);
        translate(scene, trans[code].move);
        break;
    case RESET:
    {
        printf("reset\n");
        frame = 0;
        free(sum);
        sum = NULL;
        scene->camera = (Vec3){0, 0, FOCAL_LEN};
        scene->cam_dir = (Vec3){0, 0, -1};
        y_rotation = 0;
        x_rotation = 0;
        break;
    }
    default:
        printf("keyboard: %d\n", code);
        return 0;
    }
    init(win);
    printf("Pos (%f, %f, %f)\n", scene->camera.x, scene->camera.y, scene->camera.z);
    return (0);
}

int on_mouse_move(int x, int y, Win *win)
{
    Scene *scene = &win->scene;
    if (is_mouse_down) // x >= 0 && x <= win->width && y >= 0 && y <= win->height)
    {
        int dx = x - old_x;
        int dy = y - old_y;
        frame = 0;
        free(sum);
        sum = NULL;
        x_rotation += -dx * 0.25f;
        y_rotation += -dy * 0.25f;
        init(win);
    }
    old_x = x;
    old_y = y;
    // printf("mouse move (%d, %d)\n", x, y);
    return 0;
}

int on_mouse_up(int code, int x, int y, Win *win)
{
    if (code == MOUSE_LEFT)
        is_mouse_down = 0;
    return 0;
}
int on_mouse_down(int code, int x, int y, Win *win)
{
    Scene *scene = &win->scene;
    struct
    {
        Vec3 move;
        char *msg;
    } trans[1000] = {
        [SCROLL_UP] = {(Vec3){0, 0, -.1}, "forward"},
        [SCROLL_DOWN] = {(Vec3){0, 0, .1}, "backward"},
    };
    switch (code)
    {
    case MOUSE_LEFT:
        is_mouse_down = 1;
        break;
    case SCROLL_UP:
    case SCROLL_DOWN:
        printf("%s\n", trans[code].msg);
        translate(scene, trans[code].move);
        break;
    default:
        printf("mouse down: %d, (%d, %d)\n", code, x, y);
        return 0;
    }
    init(win);
    return 0;
}

bool recording = true;
unsigned int *FramesList[FRAMES_LEN];
int frame_shots;

int draw(void *ptr)
{
    Win *win = (Win *)ptr;
    Scene *scene = &win->scene;
    struct timespec time_start, time_end;
    float frame_time;
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    if (recording)
    {
        FramesList[frame_shots] = calloc(win->width * win->height, sizeof(int));
        frame_shots++;
    }
    frame++;
    if (sum == NULL)
        sum = calloc(win->width * win->height, sizeof(Color));
    if (recording && frame_shots < FRAMES_LEN)
    {
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
                FramesList[frame_shots - 1][h * win->width + w] = ((int)(255.999 * sqrtf(pixel.r)) << 16) | ((int)(255.999 * sqrtf(pixel.g)) << 8) | ((int)(255.999 * sqrtf(pixel.b)));

                char *dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel / 8));
                *(unsigned int *)dst = ((int)(255.999 * sqrtf(pixel.r)) << 16) | ((int)(255.999 * sqrtf(pixel.g)) << 8) | ((int)(255.999 * sqrtf(pixel.b)));
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &time_end);
        frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
        if (frame_shots % 10 == 0)
            printf("%6.2f: save frame %d\n", frame_time, frame_shots);
    }
    else if (recording)
    {
        recording = false;
        frame_shots = 0;
    }
    else
    {
        // get images from FramesList to win->addr
        for (int h = 0; h < win->height; h++)
        {
            for (int w = 0; w < win->width; w++)
            {
                char *dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel / 8));
                *(unsigned int *)dst = FramesList[frame_shots][h * win->width + w];
            }
        }
        frame_shots++;
        if (frame_shots > FRAMES_LEN - 2)
            frame_shots = 0;

        clock_gettime(CLOCK_MONOTONIC, &time_end);
        frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
        if (frame_shots % 10 == 0)
            printf("%6.2f: render frame %d\n", frame_time, frame_shots);
    }
    mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
    return 0;
}

void init(Win *win)
{
    Scene *scene = &win->scene;
    scene->view_angle = degrees_to_radians(60); // TODO: maybe it's useless
    /*
        translation: transl camera and cam_dir   (key board)
        rotation:    rotate cam_dir over camera, (listen_on_mouse)
    */
    scene->cam_dir = rotate(degrees_to_radians(x_rotation), (Vec3){0, 0, -1}, 'y');
    scene->cam_dir = rotate(degrees_to_radians(y_rotation), scene->cam_dir, 'x');
    scene->w = mul_vec3(-1, unit_vector(scene->cam_dir)); // step in z axis and z+
    Vec3 upv = (Vec3){0, 1, 0};                           // used for getting u,v
    upv = rotate(degrees_to_radians(x_rotation), upv, 'y');
    upv = rotate(degrees_to_radians(y_rotation), upv, 'x');

    scene->len = 1; // length(sub_vec3(scene->camera, scene->cam_dir));    // distance from screen
    float tang = tan(scene->view_angle / 2);
    float screen_height = 2 * tang * scene->len;
    float screen_width = screen_height * ((float)win->width / win->height);

    scene->u = unit_vector(cross_(upv, scene->w));      // x+ (get v vector)
    scene->v = unit_vector(cross_(scene->w, scene->u)); // y+ (get u vector)

    // viewport steps
    scene->screen_u = mul_vec3(screen_width, scene->u);
    scene->screen_v = mul_vec3(-screen_height, scene->v);
    // window steps
    scene->pixel_u = div_vec3(scene->screen_u, win->width);
    scene->pixel_v = div_vec3(scene->screen_v, win->height);

    Vec3 screen_center = add_vec3(scene->camera, mul_vec3(-scene->len, scene->w));
    Vec3 upper_left = sub_vec3(screen_center, div_vec3(add_vec3(scene->screen_u, scene->screen_v), 2)); // center - screen_u / 2 - screen_v / 2 = center - (screen_u+screen_v)/2
    scene->first_pixel = add_vec3(upper_left, div_vec3(add_vec3(scene->pixel_u, scene->pixel_v), 2));   // upper_left + (pixel_u + pixel_v) / 2
}

int main(void)
{
    Win win = {0};
    win.width = 512;
    win.height = (int)((float)win.width / 1);
    if (win.height < 1)
        win.height = 1;
    // Scene *scene = &win.scene;
    win.scene.objects = calloc(300000, sizeof(Obj));
    win.scene.camera = (Vec3){0, 0, FOCAL_LEN};
    win.scene.cam_dir = (Vec3){0, 0, -1};
    init(&win);
#if 0
    parse_obj(&win.scene, "cube.obj");
#elif 0
    // // add objects
    Vec3 p1, p2, p3;
    p1 = (Vec3){0, 0, 0};
    p2 = (Vec3){-1, 0, 0};
    p3 = (Vec3){0, -1, 0};
    win.scene.objects[win.scene.pos++] = new_triangle(p1, p2, p3, (Color){1, 0, 0}, Abs_);
#else
    struct
    {
        Vec3 normal;
        float dist; // distance from camera
        Mat mat;
    } plans[] = {
        {(Vec3){0, -1, 0}, -4, Abs_}, // up
        {(Vec3){0, 1, 0}, -4, Abs_},  // down
        {(Vec3){0, 0, 1}, -12, Abs_}, // behind
        {(Vec3){1, 0, 0}, -4, Abs_},  // right
        {(Vec3){-1, 0, 0}, -4, Abs_}, // left
        {(Vec3){}, 0, 0},
    };
    struct
    {
        Vec3 org;
        float rad;
        Mat mat;
    } spheres[] = {
        {(Vec3){0, 1, -2}, .5, Refl_},
        {(Vec3){-1, 0, -1}, .5, Refl_},
        {(Vec3){1, 0, -1}, .5, Refl_},
        {(Vec3){0, -.5, -2}, .5, Refl_},
        {(Vec3){0, -5, -.5}, .5, Refl_},
        {(Vec3){}, 0, (Mat)0},
    };
    Color colors[] = COLORS;
    int i = 0;
    while (spheres[i].mat)
    {
        Vec3 org = spheres[i].org;
        float rad = spheres[i].rad;
        Mat mat = spheres[i].mat;
        win.scene.objects[win.scene.pos] = new_sphere(org, rad, colors[win.scene.pos % (sizeof(colors) / sizeof(*colors))], mat);

        i++;
        win.scene.pos++;
    }
    i = 0;
    while (plans[i].mat)
    {
        Vec3 normal = plans[i].normal;
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
   
    // win.scene.objects[win.scene.pos++] = new_sphere((Vec3){0, 0, 1}, 1, (Color){1, 0, 0}, Abs_);
#endif

    win.mlx = mlx_init();
    win.win = mlx_new_window(win.mlx, win.width, win.height, "Mini Raytracer");
    win.img = mlx_new_image(win.mlx, win.width, win.height);
    win.addr = mlx_get_data_addr(win.img, &win.bits_per_pixel, &win.line_length, &win.endian);

    mlx_loop_hook(win.mlx, draw, &win);
    unsigned mouse_mask = (1 << 15) | (1 << 0) | (1 << 2) | (1 << 19) | (1 << 3) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 8) | (1 << 16) | (1 << 23);
    mlx_hook(win.win, 2, 1L << 0, listen_on_key, &win);
    mlx_hook(win.win, 4, mouse_mask, on_mouse_down, &win);
    mlx_hook(win.win, 6, mouse_mask, on_mouse_move, &win);

    mlx_loop(win.mlx);
}
