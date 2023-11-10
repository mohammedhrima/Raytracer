#include "utils.cpp"

// globals
extern _Atomic int frame_index;
int is_mouse_down;
int frame_shots;
float y_rotation;
float x_rotation;
int old_x;
int old_y;
Obj *Global;

void translate(Win *win, Vec3 move)
{
    Vec3 x = move.x * win->scene.u;
    Vec3 y = move.y * win->scene.v;
    Vec3 z = move.z * win->scene.w;
    win->scene.camera = win->scene.camera + x + y + z;
}

Vec3 rotate(Vec3 u, int axes, float angle)
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
        std::cerr << "Error in rotation" << std::endl;
        exit(0);
    }
    }
    return (Vec3){};
}

void init(Win *win)
{
    Scene *scene = &win->scene;
    frame_index = 1;

    scene->cam_dir = rotate((Vec3){0, -0.2, -1}, 'x', y_rotation);
    scene->cam_dir = rotate(scene->cam_dir, 'y', x_rotation);

#if 0
    scene->w = -1 * unit_vector(scene->cam_dir); // step in z axis and z
    Vec3 upv = rotate((Vec3){0, 1, 0}, 'x', y_rotation);
    upv = rotate(upv, 'y', x_rotation);
#else
    scene->w = -1 * unit_vector(scene->cam_dir); // step in z axis and z
    Vec3 upv = (Vec3){0, 1, 0};
#endif
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
    Scene *scene = &win->scene;
    if (is_mouse_down)
    {
        int dx = x - old_x;
        int dy = y - old_y;

        x_rotation -= ((float)dx / (win->width * 0.5f)) * PI * 0.5f;
        y_rotation -= ((float)dy / (win->height * 0.5f)) * PI * 0.5f;
    }
    old_x = x;
    old_y = y;
    std::cerr << "Error: mouse move isn't set" << std::endl;
    return 0;
}

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

float hit_sphere(Ray *ray, Obj obj, float max)
{
    Vec3 oc = ray->org - obj.center;

    float a = length_squared(ray->dir);
    float half_b = dot(oc, ray->dir);
    float c = length_squared(oc) - obj.radius * obj.radius;
    float delta = half_b * half_b - a * c;
    if (delta < 0)
        return -1.0;
    delta = sqrtf(delta);
    float sol = (-half_b - delta) / a;
    if (sol <= ZERO || sol >= max)
        sol = (-half_b + delta) / a;
    if (sol <= ZERO || sol >= max)
        return -1.0;
    return (sol);
}

float hit_plan(Ray *ray, Vec3 normal, float d, float max)
{
    float t = -d - dot(normal, ray->org);
    float div = dot(ray->dir, normal);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= ZERO || t >= max)
        return -1.0;
    return (t);
}

float hit_triangle(Ray *ray, Obj trian, float max)
{
    float t = dot(trian.normal, (trian.p1 - ray->org));
    float div = dot(trian.normal, ray->dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= ZERO || t >= max)
        return -1.0;

    Vec3 p = point_at(ray, t) - trian.p1;
    Vec3 u = trian.p2 - trian.p1;
    Vec3 v = trian.p3 - trian.p1;
    Vec3 w = trian.normal / dot(trian.normal, trian.normal);
    float alpha = dot(cross(p, v), w);
    float beta = -dot(cross(p, u), w);
    if (alpha < 0 || beta < 0 || alpha + beta > 1.0)
        return -1.0;
    return t;
}

float hit_rectangle(Ray *ray, Obj rec, float max)
{
    float t = dot(rec.normal, (rec.p1 - ray->org));
    float div = dot(rec.normal, ray->dir);
    if (fabsf(div) <= ZERO)
        return -1.0;
    t /= div;
    if (t <= ZERO || t >= max)
        return -1.0;

    float len = length(rec.normal);
    Vec3 w = point_at(ray, t) - rec.p1;
    Vec3 u = rec.p2 - rec.p1;
    Vec3 v = rec.p3 - rec.p1;
    float beta = -dot(cross(w, u), rec.normal) / (len * len);
    float alpha = dot(cross(w, v), rec.normal) / (len * len);
    if (alpha < 0 || beta < 0 || alpha > 1.0 || beta > 1.0)
        return -1.0;

    return t;
}

Vec3 ranv(Vec3 normal)
{
    Vec3 ran = {0, 0, 1};
    if (fasbf(dot(ran, normal)) >= 0.9f)
        ran = {1, 0, 0};
    return ran;
}

float hit_cylinder(Ray *ray, Obj cylin, float max, Vec3 *hit_normal)
{
    Vec3 ran = ranv(cylin.normal);
    Vec3 u = unit_vector(cylin.normal, ran);
    Vec3 v = unit_vector(cylin.normal, u);

    float a = pow2(dot(u, ray->dir)) + pow2(dot(v, ray->dir));
    float b = 2 * (dot((ray->org - cylin.center), u) * dot(u, ray->dir) + dot((ray->org - cylin.center), v) * dot(v, ray->dir));
    float c = pow2(dot((ray->org - cylin.center), u)) + pow2(dot((ray->org - cylin.center), v)) - pow2(cylin.radius);

    float delta = b * b - 4 * a * c;
    float x = -1.0;
    // hit cylinder side
    if (delta >= 0)
    {
        delta = sqrt(delta);
        float x1 = (-b - delta) / (2 * a);
        float x2 = (-b + delta) / (2 * a);
        if(x1 > ZERO && x1 < max)
            x = x1;
        if(x2 > ZERO && x2 < max && (x < 0 || x2 < x))
            x = x2;
        Vec3 hit_point = point_at(ray, x);
        float h = fabsf(dot(cylin.normal, hit_point - cylin.center));
        if(h > cylin.height / 2)
            x = -1.0;
        *hit_normal = p - (dot(p - center, normal) * normal )
    }
}