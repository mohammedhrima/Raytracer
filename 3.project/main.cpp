#include "utils.cpp"

void add_objects(Win *win)
{
    struct
    {
        Vec3 center;
        float radius;
    } spheres[] = {
        {(Vec3){-2.5, -2.5, 0.0}, .1},
        {(Vec3){1.4, 1.4, -3.0}, 5.},
        {(Vec3){+1, -3, +0.5}, 0.},
        {(Vec3){+0, +1, -3.0}, 1.},
        {(Vec3){+1, -1, -3.0}, 1.},
    };
#if 1
    // win->scene.objects[win->scene.pos++] = new_plan({+1, +0, +0}, 10, COLORS[win->scene.pos], Abs_); // left
    // win->scene.objects[win->scene.pos++] = new_plan({-1, +0, +0}, 4, COLORS[win->scene.pos], Abs_);   // right
    // win->scene.objects[win->scene.pos++] = new_plan({+0, +1, +0}, 10, COLORS[win->scene.pos], Abs_); // down
    // win->scene.objects[win->scene.pos++] = new_plan({+0, -1, +0}, 4, COLORS[win->scene.pos], Abs_);   // up
    // win->scene.objects[win->scene.pos++] = new_plan({+0, +0, 10}, 20, COLORS[win->scene.pos], Abs_); // forward
    // win->scene.objects[win->scene.pos++] = new_plan({+0, +0, -10}, 20, COLORS[win->scene.pos], Abs_); // backward
#endif

#if 1
    int i = 0;
    while (spheres[i].radius > 0.0)
    {
        Vec3 center = spheres[i].center;
        float radius = spheres[i].radius;
        Color color = COLORS[win->scene.pos ];
        win->scene.objects[win->scene.pos] = new_sphere(center, radius, color, Abs_);
        win->scene.pos++;
        i++;
    }
    // win->scene.objects[win->scene.pos++] = new_cone((Vec3){0, 0, -3}, 2, 2, (Vec3){0, 1, 0}, COLORS[win->scene.pos ], Abs_);
#else
#endif
    // win->scene.objects[win->scene.pos++] = new_sphere((Vec3){-10, -2, 0}, 2, (Color){1, 0, 0}, Abs_);
    // win->scene.objects[win->scene.pos - 1]->lightness = 1.0;
    // win->scene.objects[win->scene.pos++] = new_sphere((Vec3){-4, 0, -1}, 2, COLORS[win->scene.pos], Abs_);
    // win->scene.objects[win->scene.pos - 1]->lightness = 150.0;
    // win->scene.objects[win->scene.pos++] = new_cylinder((Vec3){0, 0, -5}, 2, 20, (Vec3){0, 1, 0}, (Color){1, 0, 0}, Abs_);
}

// dimentions
#define WIDTH 400
#define HEIGHT WIDTH

int main(void)
{
    Win *win = new_window(WIDTH, HEIGHT, (char *)"Mini Raytracer");
    win->scene.objects = (Obj **)calloc(100, sizeof(Obj **));

    time_t time_start, time_end;
    init(win);
    add_objects(win);

#if THREADS_LEN
    int cols = ceil(sqrt(THREADS_LEN));
    int rows = ceil((float)THREADS_LEN / cols);
    for (int i = 0; i < THREADS_LEN; i++)
        win->thread_finished[i] = 1;

    Multi *multis[THREADS_LEN];
    for (int i = 0; i < THREADS_LEN; i++)
    {
        multis[i] = new_multi(win, i, cols, rows);
        pthread_create(&multis[i]->thread, nullptr, TraceRay, multis[i]);
    }
#else
    TraceRay(win);
#endif
    int quit = 0;
    while (!quit)
    {
        listen_on_events(win, quit);
        time_start = get_time();
#if THREADS_LEN
        for (int i = 0; i < THREADS_LEN; i++)
            win->thread_finished[i] = 0;
        while (1)
        {
            int finished = 1;

            for (int i = 0; i < THREADS_LEN; i++)
            {
                if (!win->thread_finished[i])
                    finished = 0;
            }
            if (finished)
                break;
            usleep(1000);
        }
#endif
        update_window(win);
        frame_index++;
        time_end = get_time();
        std::string dynamicTitle = std::to_string(time_end - time_start) + std::string(" ms");
        SDL_SetWindowTitle(win->window, dynamicTitle.c_str());
    }
    close_window(win);
}