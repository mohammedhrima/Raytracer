#include "00-utils.cpp"

int frame;
Color *sum;
int curr_x;
int curr_y;
int old_x;
int old_y;
float y_rotation = 0;
float x_rotation = 0;
int is_mouse_down = 0;

void draw(Win *win)
{
    Scene *scene = &win->scene;
    struct timespec time_start, time_end;
    static int frame;
    float frame_time;

    frame++;
     clock_gettime(CLOCK_MONOTONIC, &time_start);
    if (sum == NULL)
        sum = (Color *)calloc(win->width * win->height, sizeof(Color));
    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            Vec3 pixel_center = scene->first_pixel + (w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
            Vec3 dir = pixel_center - scene->camera;
            Ray ray = (Ray){.org = scene->camera, .dir = dir};
            Color pixel = ray_color(win, ray, 5);
            sum[h * win->width + w] = sum[h * win->width + w] + pixel;
            pixel = sum[h * win->width + w] / (float)frame;
            if (pixel.r > 1)
                pixel.r = 1;
            if (pixel.g > 1)
                pixel.g = 1;
            if (pixel.b > 1)
                pixel.b = 1;
            win->pixels[h * win->width + w] = ((int)(255.999 * sqrtf(pixel.r)) << 16) | ((int)(255.999 * sqrtf(pixel.g)) << 8) | ((int)(255.999 * sqrtf(pixel.b)));
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
    // if (frame % 10 == 0)
        printf("%6.2f: render frame %d\n", frame_time, frame);
    update_window(win);
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
    scene->w = -1 * unit_vector(scene->cam_dir); // step in z axis and z

    Vec3 upv = (Vec3){0, 1, 0}; // used for getting u,v
    upv = rotate(degrees_to_radians(x_rotation), upv, 'y');
    upv = rotate(degrees_to_radians(y_rotation), upv, 'x');

    scene->len = 1; // TODO: maybe it's useless 
    float tang = tan(scene->view_angle / 2);
    float screen_height = 2 * tang * scene->len;
    float screen_width = screen_height * ((float)win->width / win->height);

    scene->u = unit_vector(cross_(upv, scene->w));      // x+ (get v vector)
    scene->v = unit_vector(cross_(scene->w, scene->u)); // y+ (get u vector)

    // viewport steps
    scene->screen_u = screen_width * scene->u;
    scene->screen_v = -screen_height * scene->v;
    // window steps
    scene->pixel_u = scene->screen_u / win->width;
    scene->pixel_v = scene->screen_v / win->height;

    Vec3 screen_center = scene->camera + (-scene->len * scene->w);
    Vec3 upper_left = screen_center - (scene->screen_u + scene->screen_v) / 2;
    scene->first_pixel = upper_left + (scene->pixel_u + scene->pixel_v) / 2; 
}

int main(void)
{
    int width = 512;
    int height = width / 1;
    if (height < 1)
        height = 1;
    Win *win = new_window(width, height, (char *)"Mini Raytracer");
    win->scene.objects = (Obj *)calloc(100, sizeof(Obj));
    win->scene.camera = (Vec3){0, 0, FOCAL_LEN};
    win->scene.cam_dir = (Vec3){0, 0, -1};
    init(win);

#if 1
    struct
    {
        Vec3 normal;
        float dist; // distance from camera
        Mat mat;
    } plans[] = {
        {(Vec3){0, -1, 0}, -4, Abs_},  // up
        {(Vec3){0, 1, 0}, -4, Abs_},   // down
        {(Vec3){0, 0, 1}, -12, Abs_}, // behind
        {(Vec3){1, 0, 0}, -4, Abs_},   // right
        {(Vec3){-1, 0, 0}, -4, Abs_},  // left
        {(Vec3){}, 0, (Mat)0},
    };
    struct
    {
        Vec3 org;
        float rad;
        Mat mat;
    } spheres[] = {
        {(Vec3){0, 1, -2}, .5, Abs_},
        {(Vec3){-1, 0, -1}, .5, Abs_},
        {(Vec3){1, 0, -1}, .5, Abs_},
        {(Vec3){0, -.5, -2}, .5, Abs_},
        {(Vec3){0, -5, -.5}, .5, Abs_},
        {(Vec3){}, 0, (Mat)0},
    };
    Color colors[] = COLORS;
    int i = 0;
    while (spheres[i].mat)
    {
        Vec3 org = spheres[i].org;
        float rad = spheres[i].rad;
        Mat mat = spheres[i].mat;
        win->scene.objects[win->scene.pos] = new_sphere(org, rad, colors[win->scene.pos % (sizeof(colors) / sizeof(*colors))], mat);
        i++;
        win->scene.pos++;
    }
    i = 0;
    while (plans[i].mat)
    {
        Vec3 normal = plans[i].normal;
        float dist = plans[i].dist;
        Mat mat = plans[i].mat;
        win->scene.objects[win->scene.pos] = new_plan(normal, dist, colors[win->scene.pos % (sizeof(colors) / sizeof(*colors))], mat);
        if (i == 0)
        {
            win->scene.objects[win->scene.pos].light_intensity = 2;
            win->scene.objects[win->scene.pos].light_color = (Color){1, 1, 1};
        }
        i++;
        win->scene.pos++;
    }
#endif
    while (!glfwWindowShouldClose(win->window))
        draw(win);
    glfwTerminate();
    return 0;
}
