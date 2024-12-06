#include "header.hpp"

// globals
int is_mouse_down;
int frame_shots;
float y_rotation;
float x_rotation;
int old_x;
int old_y;


// raytracing
void init(Win *win)
{
    Scene *scene = &win->scene;
    frame_index = 1;

    scene->cam_dir = rotate((Vec3){0, -0.2, -1}, 'x', y_rotation);
    scene->cam_dir = rotate(scene->cam_dir, 'y', x_rotation);

    scene->w = -1 * unit_vector(scene->cam_dir); // step in z axis and z
    Vec3 upv = rotate((Vec3){0, 1, 0}, 'x', y_rotation);
    upv = rotate(upv, 'y', x_rotation);

    float screen_height = 2 * tan(degrees_to_radians(FOV / 2)) * FOCAL_LEN;
    float screen_width = screen_height * ((float)win->width / win->height);

    scene->u = unit_vector(cross(upv, scene->w));      // x+ (get u vector)
    scene->v = unit_vector(cross(scene->w, scene->u)); // y+ (get v vector)

    // viewport steps
    Vec3 screen_u = screen_width * scene->u;
    Vec3 screen_v = -screen_height * scene->v;
    // window steps
    scene->pixel_u = screen_u / win->width;
    scene->pixel_v = screen_v / win->height;

    Vec3 screen_center = scene->camera + (-FOCAL_LEN * scene->w);
    Vec3 upper_left = screen_center - (screen_u + screen_v) / 2;
    scene->first_pixel = upper_left + (scene->pixel_u + scene->pixel_v) / 2;
}

int on_mouse_move(Win *win, int x, int y)
{
    // Scene *scene = &win->scene;
    if (is_mouse_down)
    {
        int dx = x - old_x;
        int dy = y - old_y;

        x_rotation -= ((float)dx / (win->width * 0.5f)) * PI * 0.5f;
        y_rotation -= ((float)dy / (win->height * 0.5f)) * PI * 0.5f;
    }
    old_x = x;
    old_y = y;
    return 0;
}

Obj *Global;
void listen_on_events(Win *win, int &quit)
{
    struct
    {
        Vec3 move;
        char *msg;
    } trans[1000];
    trans[FORWARD - 1073741900] = {(Vec3){0, 0, -1}, (char *)"forward"};
    trans[BACKWARD - 1073741900] = {(Vec3){0, 0, 1}, (char *)"backward"};
    trans[UP - 1073741900] = {(Vec3){0, 1, 0}, (char *)"up"};
    trans[DOWN - 1073741900] = {(Vec3){0, -1, 0}, (char *)"down"};
    trans[LEFT - 1073741900] = {(Vec3){-1, 0, 0}, (char *)"left"};
    trans[RIGHT - 1073741900] = {(Vec3){1, 0, 0}, (char *)"right"};

    Vec3 move_obj[100];
    move_obj[NUM_0 - NUM_0] = (Vec3){0, 0, +1};
    move_obj[NUM_0 - NUM_5] = (Vec3){0, 0, -1};
    move_obj[NUM_0 - NUM_8] = (Vec3){0, +1, 0};
    move_obj[NUM_0 - NUM_2] = (Vec3){0, -1, 0};
    move_obj[NUM_0 - NUM_4] = (Vec3){-1, 0, 0};
    move_obj[NUM_0 - NUM_6] = (Vec3){+1, 0, 0};

    Scene *scene = &win->scene;
    while (SDL_PollEvent(&win->ev) != 0)
    {
        switch (win->ev.type)
        {
        case QUIT:
            quit = true;
            break;
#if THREADS_LEN
        case MOUSE_DOWN:
            if (win->ev.button.button == MOUSE_LEFT)
                std::cout << "Left mouse button clicked at (" << win->ev.button.x << ", " << win->ev.button.y << ")" << std::endl;
            is_mouse_down = 1;
            old_x = win->ev.button.x;
            old_y = win->ev.button.y;
            break;
        case MOUSE_UP:
            is_mouse_down = 0;
            break;
        case MOUSE_MOTION:
            on_mouse_move(win, win->ev.motion.x, win->ev.motion.y);
            if (is_mouse_down)
            {
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                init(win);
            }
            break;
        case MOUSE_SCROLL:
            if (win->ev.wheel.y > 0)
                translate(win, 2 * trans[FORWARD - 1073741900].move);
            else
                translate(win, 2 * trans[BACKWARD - 1073741900].move);
            frame_index = 1;
            memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
            init(win);
            break;
#endif
        case PRESS:
            switch (win->ev.key.keysym.sym)
            {
            case ESC:
                quit = true;
                break;
#if THREADS_LEN
            case UP:
            case DOWN:
            case LEFT:
            case RIGHT:
                translate(win, trans[win->ev.key.keysym.sym - 1073741900].move);
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                init(win);
                break;
            case RESET:
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                x_rotation = 0;
                y_rotation = 0;
                scene->camera = (Vec3){0, 0, FOCAL_LEN};
                init(win);
                std::cout << "Reset" << std::endl;
                break;
            case KEY_0:
            case KEY_1:
            case KEY_2:
            case KEY_3:
            case KEY_4:
            case KEY_5:
            case KEY_6:
            case KEY_7:
            case KEY_8:
            case KEY_9:
                if (win->scene.objects[win->ev.key.keysym.sym - KEY_0])
                {
                    Global = win->scene.objects[win->ev.key.keysym.sym - KEY_0];
                    std::cout << "Select object: " << win->ev.key.keysym.sym - KEY_0 << std::endl;
                }
                else
                    std::cout << "index out of range" << std::endl;
                break;
            case NUM_0:
            case NUM_5:
            case NUM_2:
            case NUM_4:
            case NUM_6:
            case NUM_8:
                Global->center = Global->center + move_obj[NUM_0 - win->ev.key.keysym.sym];
                std::cout << "move with: " << move_obj[NUM_0 - win->ev.key.keysym.sym] << std::endl;
                frame_index = 1;
                memset(win->scene.sum, SDL_COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
                init(win);
                break;
#endif
            default:
                std::cout << "key cliqued: " << win->ev.key.keysym.sym << std::endl;
                break;
            }
            break;
        default:
            break;
        }
    }
}

float hit_sphere(Ray *ray, Vec3 center, float radius, float min, float max)
{
    Vec3 oc = ray->org - center;

    float a = length_squared(ray->dir);
    float half_b = dot(oc, ray->dir);
    float c = length_squared(oc) - radius * radius;
    float delta = half_b * half_b - a * c;
    if (delta < 0)
        return -1.0;
    delta = sqrtf(delta);
    float sol = (-half_b - delta) / a;
    if (sol <= min || sol >= max)
        sol = (-half_b + delta) / a;
    if (sol <= min || sol >= max)
        return -1.0;
    return (sol);
}

float hit_plan(Ray *ray, Vec3 normal, float d, float min, float max)
{
    float t = -d - dot(normal, ray->org);
    float div = dot(ray->dir, normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;
    return (t);
}

float hit_triangle(Ray *ray, Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, float min, float max)
{
    float t = dot(normal, (p1 - ray->org));
    float div = dot(normal, ray->dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;

    Vec3 p = point_at(ray, t) - p1;
    Vec3 u = p2 - p1;
    Vec3 v = p3 - p1;
    Vec3 w = normal / dot(normal, normal);
    float alpha = dot(cross(p, v), w);
    float beta = -dot(cross(p, u), w);
    if (alpha < 0 || beta < 0 || alpha + beta > 1.0)
        return -1.0;
    return t;
}

float hit_rectangle(Ray *ray, Vec3 p1, Vec3 p2, Vec3 p3, Vec3 normal, float min, float max)
{
    float t = dot(normal, (p1 - ray->org));
    float div = dot(normal, ray->dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= min || t >= max)
        return -1.0;

    float len = length(normal);
    Vec3 w = point_at(ray, t) - p1;
    Vec3 u = p2 - p1;
    Vec3 v = p3 - p1;
    float beta = -dot(cross(w, u), normal) / (len * len);
    float alpha = dot(cross(w, v), normal) / (len * len);
    if (alpha < 0 || beta < 0 || alpha > 1.0 || beta > 1.0)
        return -1.0;

    return t;
}

#if 1
float hit_cylinder(Ray *ray, Vec3 center, float radius, float height, Vec3 normal, float min, float max, Vec3 *hit_normal)
{
    Vec3 ran = {0, 0, 1};
    if (fabsf(dot(ran, normal)) >= 0.9f)
        ran = {1, 0, 0};
    Vec3 u = unit_vector(cross(ran, normal));
    Vec3 v = unit_vector(cross(normal, u));

    float a = pow2(dot(u, ray->dir)) + pow2(dot(v, ray->dir));
    float b = 2 * (dot((ray->org - center), u) * dot(u, ray->dir) + dot((ray->org - center), v) * dot(v, ray->dir));
    float c = pow2(dot((ray->org - center), u)) + pow2(dot((ray->org - center), v)) - pow2(radius);

    float delta = b * b - 4 * a * c;
    float x = -1.0;
    if (delta >= 0)
    {
        delta = sqrt(delta);
        // hit cylinder
        float x1 = (-b + delta) / (2 * a);
        float x2 = (-b - delta) / (2 * a);

        if (x1 > min && x1 < max)
            x = x1;
        if (x2 > min && x2 < max && (x < 0 || x2 < x))
            x = x2;

        float cal = fabsf(dot(normal, point_at(ray, x) - center));
        if (cal > height / 2)
            x = -1.0;

        Vec3 p = point_at(ray, x);
        *hit_normal = p - (dot(p - center, normal) * normal + center);
    }

    Vec3 c1 = center + (height / 2) * normal;
    float d1 = hit_plan(ray, normal, -dot(c1, normal), min, max);
    if (d1 > 0)
    {
        Vec3 p1 = point_at(ray, d1);
        if (length(p1 - c1) <= radius && (x < 0 || d1 < x))
        {
            x = d1;
            *hit_normal = normal;
        }
    }

    Vec3 c2 = center - (height / 2) * normal;
    float d2 = hit_plan(ray, -1 * normal, -dot(c2, -1 * normal), min, max);
    if (d2 > 0)
    {
        Vec3 p1 = point_at(ray, d2);
        if (length(p1 - c2) <= radius && (x < 0 || d2 < x))
        {
            x = d2;
            *hit_normal = -1 * normal;
        }
    }
    if (x < min || x > max)
        return -1;
    return x;
}
#else

float hit_cylinder(Ray *ray, Obj cylin, float min, float max, Vec3 *hit_normal)
{
    Vec3 ran = (Vec3){0, 0, 1};
    if (fabsf(dot(ran, cylin.normal)) >= 0.9f)
        ran = (Vec3){1, 0, 0};
    Vec3 u = unit_vector(cross(ran, cylin.normal));
    Vec3 v = unit_vector(cross(cylin.normal, u));

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
        Vec3 p = point_at(ray, x);
        float d = dot(calc(p, '-', cylin.center));
        *hit_normal = calc(calc(p, '-', scale(cylin.normal, '*', dot(calc(p, '-', cylin.center), cylin.normal))), '+', cylin.center);
    }

    Vec3 c1 = calc(cylin.center, '+', scale(cylin.normal, '*', cylin.height / 2));
    float d1 = hit_plan(ray, cylin.normal, -dot(c1, cylin.normal), min, max);
    if (d1 > 0)
    {
        Vec3 p1 = point_at(ray, d1);
        if (length(calc(p1, '-', c1)) <= cylin.radius && (x < 0 || d1 < x))
        {
            x = d1;
            *hit_normal = cylin.normal;
        }
    }

    Vec3 c2 = calc(cylin.center, '-', scale(cylin.normal, '*', cylin.height / 2));
    float d2 = hit_plan(ray, scale(cylin.normal, '*', -1), dot(c2, scale(cylin.normal, '*', -1)), min, max);
    if (d2 > 0)
    {
        Vec3 p1 = point_at(ray, d2);
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
#endif

float hit_cone(Ray *ray, Vec3 center, Vec3 normal, float radius, float height, float min, float max)
{
    Vec3 ran = {0, 1, 0};
    if (fabsf(dot(ran, normal)) >= 0.9f)
        ran = {0, 0, 1};
    Vec3 u = unit_vector(cross(ran, normal));
    Vec3 v = unit_vector(cross(normal, u));

    float w = pow2(radius) / pow2(height);

    Vec3 org = ray->org - center;
#if 0
    float a = pow2(dot(ray->dir, u)) + pow2(dot(ray->dir, v)) - pow2(radius) * pow2(dot(ray->dir, normal)) / pow2(height);
    float b = 2 * dot(org, u) * dot(ray->dir, u) + 2 * dot(org, v) * dot(ray->dir, v) - ( w * 2 * dot(ray->dir, normal) * dot(org, normal)) + (2 * pow2(radius) / height) * dot(ray->dir, normal);
    float c = pow2(dot(org, u)) + pow2(dot(org, v)) - pow2(radius) - pow2(radius) * pow2(dot(org, normal)) / pow2(height) - (2 * pow2(radius) / height) * dot(org, normal);
#else
    float a = pow2(dot(ray->dir, u)) + pow2(dot(ray->dir, v)) - w * pow2(dot(ray->dir, normal));
    float b = 2 * dot(org, u) * dot(ray->dir, u) + 2 * dot(org, v) * dot(ray->dir, v) - (w * 2 * dot(org, normal) * dot(ray->dir, normal) - (2 * pow2(radius) / height) * dot(ray->dir, normal));
    float c = pow2(dot(org, u)) + pow2(dot(org, v)) - pow2(radius) - (w * pow2(dot(org, normal)) - (2 * pow2(radius) / height) * dot(org, normal));
#endif

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
    // height *= 3;
    float d1 = dot(normal, point_at(ray, x1) - center);
    float d2 = dot(normal, point_at(ray, x2) - center);
    if (x1 > 0 && d1 >= 0 && d1 < height)
        x = x1;
    if (x2 > 0 && d2 >= 0 && d2 < height && (x < 0 || x2 < x))
        x = x2;

#if 0
    Vec3 c1 = center + (height / 2) * normal;
    float d1 = hit_plan(ray, normal, -dot(c1, normal), min, max);
    if (d1 > 0)
    {
        Vec3 p1 = point_at(ray, d1);
        if (length(p1 - c1) <= radius && (x < 0 || d1 < x))
            x = d1;
    }

    Vec3 c2 = center - (height / 2) * normal;
    float d2 = hit_plan(ray, -1 * normal, -dot(c2, -1 * normal), min, max);
    if (d2 > 0)
    {
        Vec3 p1 = point_at(ray, d2);
        if (length(p1 - c2) <= radius && (x < 0 || d2 < x))
            x = d2;
    }
#endif
    if (x < min || x > max)
        return -1;
    return x;
}

Ray get_new_ray(Obj *obj, Ray *ray, float closest)
{
    Vec3 norm;
    float val;
    Ray nray;

    nray.org = point_at(ray, closest);
    switch (obj->type)
    {
    case SPHERE:
        norm = unit_vector(ray->org - obj->center);
        break;
    case PLAN:
    case TRIANGLE:
    case RECTANGLE:
    case CYLINDER:
    case CONE:
        norm = obj->normal;
        break;
    default:
        std::cerr << "Unkown object type" << std::endl;
        exit(1);
        break;
    }
    bool same_dir = (dot(norm, ray->dir) >= 0);
    if (same_dir)
        norm = -1 * norm;
    if (obj->mat == REFLECT)
    {
        val = -2 * dot(ray->dir, norm);
        nray.dir = ray->dir + (val * norm);
    }
    else if (obj->mat == REFRACT)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);
        float cos_theta = dot(ray->dir, norm) / (length(ray->dir) * length(norm));
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (refraction_ratio * sin_theta > 1.0) // Reflect
        {
            val = -2 * dot(ray->dir, norm);
            nray.dir = ray->dir + (val * norm);
        }
        else // Refract
        {
            Vec3 ray_dir = unit_vector(ray->dir);
            Vec3 Perp = refraction_ratio * (ray_dir - (dot(ray_dir, norm) * norm));
            Vec3 Para = sqrt(1 - pow(length(Perp), 2)) * (-1 * norm);
            nray.dir = Perp + Para;
        }
    }
    else if (obj->mat == ABSORABLE)
        nray.dir = norm + random_unit_vector();
    else
    {
        std::cerr << "Error in get_new_ray" << std::endl;
        exit(-1);
    }
    return nray;
}

void hit_object(Scene *scene, Ray *ray, float &closest, int &hit_index, Vec3 &normal)
{
    closest = FLT_MAX;
    hit_index = -1;
    float x = 0.0;
    for (int i = 0; i < scene->pos; i++)
    {
        if (scene->objects[i]->type == PLAN)
            x = hit_plan(ray, scene->objects[i]->normal, scene->objects[i]->d, ZERO, closest);
        if (scene->objects[i]->type == SPHERE)
            x = hit_sphere(ray, scene->objects[i]->center, scene->objects[i]->radius, ZERO, closest);
        else if (scene->objects[i]->type == CYLINDER)
            x = hit_cylinder(ray, scene->objects[i]->center, scene->objects[i]->radius, scene->objects[i]->height, scene->objects[i]->normal, ZERO, closest, &normal);
        // x = hit_cylinder(ray, *(scene->objects[i]), ZERO, closest, &normal);
        else if (scene->objects[i]->type == TRIANGLE)
            x = hit_triangle(ray, scene->objects[i]->p1, scene->objects[i]->p2, scene->objects[i]->p3, scene->objects[i]->normal, ZERO, closest);
        else if (scene->objects[i]->type == RECTANGLE)
            x = hit_rectangle(ray, scene->objects[i]->p1, scene->objects[i]->p2, scene->objects[i]->p3, scene->objects[i]->normal, ZERO, closest);
        else if (scene->objects[i]->type == CONE)
            x = hit_cone(ray, scene->objects[i]->center, scene->objects[i]->normal, scene->objects[i]->radius, scene->objects[i]->height, ZERO, closest);
        if (x > 0.0)
        {
            hit_index = i;
            closest = x;
        }
    }
}

Vec3 light = {0, 10, 0};
Color ray_color(Win *win, Ray *ray, int max_depth)
{
    Scene *scene = &win->scene;
    Color ambient = 0.3 * COLOR(127., 204., 229.);
    // ambient = (Color){};
    Color color = (Color){0, 0, 0};
    if (max_depth == 0)
        return color;
    // Color color = (Vec3){203.f / 255.999, 226.f / 255.999, 255.f / 255.999};
    Vec3 normal;

    float closest = FLT_MAX;
    int hit_index = -1;

    hit_object(scene, ray, closest, hit_index, normal);

    if (hit_index != -1)
    {
        Vec3 p = point_at(ray, closest);
        switch (scene->objects[hit_index]->type)
        {
        case SPHERE:
        case CONE:
            normal = p - scene->objects[hit_index]->center;
            break;
        case CYLINDER:
            break;
        case PLAN:
            normal = scene->objects[hit_index]->normal;
            normal = (dot(ray->dir, normal) > 0) ? -1 * normal : normal;
            break;
        default:
            printf("Error in ray color\n");
            exit(1);
        }

        normal = unit_vector(normal);
        Vec3 light_dir = unit_vector(light - p);
        Ray nray = (Ray){.org = point_at(ray, closest), .dir = light_dir};
        int hit_index2;
        Vec3 temp;
        hit_object(scene, &nray, closest, hit_index2, temp);
        float d;
        if (closest < length(light - p))
            d = 0;
        else
            d = fmax(dot(normal, light_dir), 0);

        color = ((Vec3){d, d, d} + ambient) * scene->objects[hit_index]->color;
    }
    else
        color = ambient;
    return color;
}

#if THREADS_LEN
void *TraceRay(void *arg)
{
    Multi *multi = (Multi *)arg;
    Win *win = multi->win;
    Scene *scene = &win->scene;
    while (1)
    {
        if (win->thread_finished[multi->idx])
        {
            usleep(1000);
            continue;
        }
        for (int h = multi->y_start; h < multi->y_end; h++)
        {
            for (int w = multi->x_start; w < multi->x_end; w++)
            {
                Vec3 pixel_center = scene->first_pixel + ((float)w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
                Vec3 dir = pixel_center - scene->camera;
                Ray ray = (Ray){.org = scene->camera, .dir = dir};
                Color pixel = ray_color(win, &ray, 2);
                win->scene.sum[h * win->width + w] = win->scene.sum[h * win->width + w] + pixel;
                pixel = win->scene.sum[h * win->width + w] / (float)frame_index;
                if (pixel.x > 1)
                    pixel.x = 1;
                if (pixel.y > 1)
                    pixel.y = 1;
                if (pixel.z > 1)
                    pixel.z = 1;
                win->pixels[h * win->width + w] = SDL_COLOR((int)(255.999 * sqrtf(pixel.x)), (int)(255.999 * sqrtf(pixel.y)), (int)(255.999 * sqrtf(pixel.z)));
            }
        }
        win->thread_finished[multi->idx] = 1;
    }
}
#else
void TraceRay(Win *win)
{
    std::cout << "Tracing ray" << std::endl;
    Scene *scene = &win->scene;
    Color pixel;
    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            int rays_per_pixel = 32;
            pixel = (Color){0, 0, 0};
            for (int i = 0; i < rays_per_pixel; i++)
            {
                Vec3 pixel_center = scene->first_pixel + ((float)w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
                Vec3 dir = pixel_center - scene->camera;
                Ray ray = (Ray){.org = scene->camera, .dir = dir};
                pixel = pixel + ray_color(win, &ray, 2);
            }
            pixel = pixel / rays_per_pixel;
            if (pixel.x > 1)
                pixel.x = 1;
            if (pixel.y > 1)
                pixel.y = 1;
            if (pixel.z > 1)
                pixel.z = 1;
            win->pixels[h * win->width + w] = SDL_COLOR((int)(255.999 * sqrtf(pixel.x)), (int)(255.999 * sqrtf(pixel.y)), (int)(255.999 * sqrtf(pixel.z)));
        }
        std::cout << "remaining: " << win->height - h << std::endl;
    }
    std::cout << "render" << std::endl;
}

#endif

void add_objects(Win *win)
{
    struct
    {
        Vec3 center;
        float radius;
        Color color;
    } spheres[] = {
        {(Vec3){+0, 0.5, 0}, .5, COLOR(165., 12., 12.)},
        {(Vec3){-1, 0.5, 0}, .5, COLOR(40., 60., 165.)},
        {(Vec3){+1, 0.5, 0}, .5, COLOR(255., 255., 255.)},
    };
    struct
    {
        Vec3 normal;
        float d;
        Color color;
    } plans[] = {
        {(Vec3){0, 1, 0}, 2, COLOR(180., 180., 180.)},
        {(Vec3){1, 0, 0}, 2, COLOR(20., 20., 20.)},
        {(Vec3){0, 0, 1}, 8, COLOR(125., 20., 20.)},
    };
    struct
    {
        Vec3 center;
        Vec3 normal;
        float radius;
        float height;
        Color color;
    } cylins[] = {
        {(Vec3){-0.5, 0.15, 1}, (Vec3){1, 0, 0}, .3, .4, COLOR(76, 178, 51)},
        {(Vec3){1, 0.2, 1.5}, (Vec3){0, 1, 0}, .4, .7, COLOR(200, 200, 200)}};

    size_t i = 0;
#if 1
    while (i < sizeof(spheres) / sizeof(*spheres))
    {
        Vec3 center = spheres[i].center;
        float radius = spheres[i].radius;
        Color color = COLORS[win->scene.pos];
        win->scene.objects[win->scene.pos++] = new_sphere(center, radius, color, ABSORABLE);
        i++;
    }
#endif

#if 1
    i = 0;
    while (i < sizeof(plans) / sizeof(*plans))
    {
        Vec3 normal = plans[i].normal;
        float d = plans[i].d;
        Vec3 color = plans[i].color;
        win->scene.objects[win->scene.pos++] = new_plan(normal, d, color, ABSORABLE);
        i++;
    }
#endif

#if 1
    i = 0;
    while (i < sizeof(cylins) / sizeof(*cylins))
    {
        Vec3 center = cylins[i].center;
        Vec3 normal = cylins[i].normal;
        float radius = cylins[i].radius;
        float height = cylins[i].height;
        Color color = cylins[i].color;
        win->scene.objects[win->scene.pos++] = new_cylinder(center, radius, height, normal, color, ABSORABLE);
        i++;
    }
#endif
}
