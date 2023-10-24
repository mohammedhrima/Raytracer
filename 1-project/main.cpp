#include "utils.cpp"

int frame_index;
Color *sum;
int curr_x;
int curr_y;
int old_x;
int old_y;

int is_mouse_down = 0;
//     0   to width/2   &&    0     to height/2
// width/2 to width     &&    0     to height/2
//     0   to width/2   && height/2 to height
// width/2 to width     && height/2 to height

void draw(Win *win, int x_start, int y_start, int width, int height)
{
    Scene *scene = &win->scene;

    frame_index++;
    if (sum == NULL)
        sum = (Color *)calloc(win->width * win->height, sizeof(Color));
    for (int h = 0; h < win->height; h++)
    {
        for (int w = 0; w < win->width; w++)
        {
            Vec3 pixel_center = scene->first_pixel + (w + random_float(0, 1)) * scene->pixel_u + ((float)h + random_float(0, 1)) * scene->pixel_v;
            Vec3 dir = pixel_center - scene->camera;
            Ray ray = (Ray){.org = scene->camera, .dir = dir};
            Color pixel = ray_color(win, ray, 4);
            sum[h * win->width + w] = sum[h * win->width + w] + pixel;
            pixel = sum[h * win->width + w] / (float)frame_index;
            if (pixel.r > 1)
                pixel.r = 1;
            if (pixel.g > 1)
                pixel.g = 1;
            if (pixel.b > 1)
                pixel.b = 1;
            if (w >= x_start && w < x_start + width && h >= y_start && h < y_start + height)
                win->pixels[h * win->width + w] = ((int)(255.999 * sqrtf(pixel.r)) << 16) | ((int)(255.999 * sqrtf(pixel.g)) << 8) | ((int)(255.999 * sqrtf(pixel.b)));
        }
    }
}

typedef struct
{
    pthread_t thread;
    Win *win;
    int idx;
} Multi;

#define THREADS 1
#define THREADS_LEN 4

void *threadFunction(void *arg)
{
    Multi *multi = (Multi *)arg;
    Win *win = multi->win;
#if THREADS
    while (!glfwWindowShouldClose(win->window))
    {
        switch (multi->idx)
        {
        case 0:
            draw(win, 0, 0, win->width / 2, win->height / 2);
            break;
        case 1:
            draw(win, win->width / 2, 0, win->width / 2, win->height / 2);
            break;
        case 2:
            draw(win, 0, win->height / 2, win->width / 2, win->height / 2);
            break;
        case 3:
            draw(win, win->width / 2, win->height / 2, win->width / 2, win->height / 2);
            break;
        default:
            break;
        }

        usleep(20);
    }

#else
    draw(win, 0, 0, win->width, win->height);
#endif
    return nullptr;
}

Multi *new_multi(int idx, Win *win)
{
    Multi *multi = (Multi *)calloc(1, sizeof(Multi));
    multi->idx = idx;
    multi->win = win;
    return multi;
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
    add_objects(win);


    Multi *multis[THREADS_LEN + 1];
    for (int i = 0; i < THREADS_LEN; i++)
    {
        multis[i] = new_multi(i, win);
#if THREADS
        pthread_create(&multis[i]->thread, nullptr, threadFunction, multis[i]);
#endif
    }

    struct timespec time_start, time_end;
    float frame_time;
    while (!glfwWindowShouldClose(win->window))
    {
#if THREADS
        update_window(win);
#else
        struct timespec time_start, time_end;
        float frame_time;
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        threadFunction(multis[1]);
        clock_gettime(CLOCK_MONOTONIC, &time_end);
        frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
        printf("%6.2f: render frame %d\n", frame_time, frame_index);
        update_window(win);
#endif
        // clock_gettime(CLOCK_MONOTONIC, &time_start);
        // draw(win);
        // clock_gettime(CLOCK_MONOTONIC, &time_end);
        // frame_time = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
        // // if (frame_index % 10 == 0)
        // printf("%6.2f: render frame %d\n", frame_time, frame_index);
        // update_window(win);
    }

#if THREADS
    for (int i = 0; i < THREADS_LEN; i++)
        pthread_join(multis[i]->thread, nullptr);
#endif

    glfwTerminate();
    return 0;
}
