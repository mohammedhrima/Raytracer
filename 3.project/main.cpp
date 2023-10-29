#include "utils.cpp"

void add_objects(Win *win);
int main()
{
    int width = 800;
    int height = width / 1;
    if (height < 1)
        height = 1;
    time_t time_start, time_end;
    Win *win = new_window(width, height, (char *)"Mini Raytracer");
    Scene *scene = &win->scene;
    scene->objects = (Obj *)calloc(100, sizeof(Obj));

    scene->camera = (Vec3){0, 0, FOCAL_LEN};

    sum = (Color *)calloc(win->width * win->height, sizeof(Color));
    init(win);
    frame_index = 1;
    int quit = 0;
    int frame_shot = 0;
    add_objects(win);

#if THREADS_LEN
    for (int i = 0; i < THREADS_LEN; i++)
        win->thread_finished[i] = 1;
    Multi *multis[THREADS_LEN];
    for (int i = 0; i < THREADS_LEN; i++)
    {
        multis[i] = new_multi(i, win);
        pthread_create(&multis[i]->thread, nullptr, Multi_TraceRay, multis[i]);
    }
#if FRAMES_LEN
    unsigned int *FramesList[FRAMES_LEN];
#endif
#else
    // no threads
    TraceRay(win);
#endif

    int i = 0;
    while (!quit)
    {
        listen_on_events(win, quit); // to exit for example
#if THREADS_LEN
        // set all threads to not finished
        for (int i = 0; i < THREADS_LEN; i++)
            win->thread_finished[i] = 0;
        time_start = get_time();
#if FRAMES_LEN
        while (THREADS_LEN && frame_shots < FRAMES_LEN)
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
        // record animation
        if (frame_shots < FRAMES_LEN)
        {
            std::cout << "Recording frame: " << frame_shots << std::endl;
            FramesList[frame_shots] = (uint32_t *)calloc(win->width * win->height, sizeof(uint32_t));
            memcpy(FramesList[frame_shots++], win->pixels, win->width * win->height * sizeof(uint32_t));
            int i = 0;
            while (win->scene.objects[i].type == sphere_ && win->scene.objects[i].speed >= 0)
            {
                win->scene.objects[i].center = rotate(win, win->scene.objects[i].center, 'z', win->scene.objects[i].speed * degrees_to_radians(1));
                i++;
            }
            frame_index = 0;
            memset(sum, COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
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
            init(win); // to be checked
        }
        // play animation
        else
        {
            frame_index = frame_index % FRAMES_LEN;
            std::cout << "Playing frame: " << frame_index << std::endl;
            memcpy(win->pixels, FramesList[frame_index], win->width * win->height * sizeof(uint32_t));
            usleep(1000);
        }
#else
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
        init(win); // to be checked
#endif
        frame_index++;
        time_end = get_time();
        std::string dynamicTitle = std::to_string(time_end - time_start) + std::string(" ms");
        SDL_SetWindowTitle(win->window, dynamicTitle.c_str());
#endif
        update_window(win);
    }
    close_window(win);
}

void add_objects(Win *win)
{
#if 0
    struct
    {
        Vec3 center;
        float rad;
        float speed;
        Color color;
    } planets[] = {
        {(Vec3){-1, -1, -3}, 1, 0, COLORS[0]},
        {(Vec3){0, 1, -3}, 1, 0, COLORS[3]},
        {(Vec3){1, -1, -3}, 1, 0, COLORS[5]},

        {(Vec3){}, 0, -1, (Color){}},
        {(Vec3){.5, 0, -15}, .2, 1.6, color(26., 26., 26.)},    // mercury
        {(Vec3){1, 0, -15}, .2, 1.4, color(230., 230., 230.)},  // venus
        {(Vec3){1.5, 0, -15}, .2, 1.2, color(47., 106., 105.)}, // earth
        {(Vec3){2, 0, -15}, .2, 1., color(153., 61., 0.)},      // mars
        {(Vec3){2.5, 0, -15}, .2, .8, color(176., 127., 53.)},  // jupiter
        {(Vec3){3, 0, -15}, .2, .6, color(176., 143., 54.)},    // saturn
        {(Vec3){3.5, 0, -15}, .2, .4, color(85., 128., 170.)},  // uranus
        {(Vec3){4, 0, -15}, .2, .2, color(54., 104., 150.)},    // neptune
        {(Vec3){0, 0, -15}, .2, 0, color(255, 255, 255)},       // sun
    };

    int i = 0;
    while (planets[i].speed >= 0)
    {
        win->scene.objects[win->scene.pos].type = sphere_;
        win->scene.objects[win->scene.pos].mat = Abs_;
        win->scene.objects[win->scene.pos].center = planets[i].center;
        win->scene.objects[win->scene.pos].radius = planets[i].rad;
        win->scene.objects[win->scene.pos].speed = planets[i].speed;
        win->scene.objects[win->scene.pos].color = planets[i].color;
        win->scene.pos++;
        i++;
        // win->scene.objects[win->scene.pos].light_intensity = planets[i].light_intensity;
        // win->scene.objects[win->scene.pos].light_color = (Color){1, 1, 1};
    }
#else
    struct
    {
        Vec3 center;
        float rad;
        float speed;
        Color color;
    } planets[] = {
        {(Vec3){-1, -1, 3}, 1, 0, COLORS[0]},
        {(Vec3){0, 1, 3}, 1, 0, COLORS[3]},
        {(Vec3){1, -1, 3}, 1, 0, COLORS[5]},

        {(Vec3){}, 0, -1, (Color){}},
        {(Vec3){.5, 0, -15}, .2, 1.6, (Color){26., 26., 26.}},    // mercury
        {(Vec3){1, 0, -15}, .2, 1.4, (Color){230., 230., 230.}},  // venus
        {(Vec3){1.5, 0, -15}, .2, 1.2, (Color){47., 106., 105.}}, // earth
        {(Vec3){2, 0, -15}, .2, 1., (Color){153., 61., 0.}},      // mars
        {(Vec3){2.5, 0, -15}, .2, .8, (Color){176., 127., 53.}},  // jupiter
        {(Vec3){3, 0, -15}, .2, .6, (Color){176., 143., 54.}},    // saturn
        {(Vec3){3.5, 0, -15}, .2, .4, (Color){85., 128., 170.}},  // uranus
        {(Vec3){4, 0, -15}, .2, .2, (Color){54., 104., 150.}},    // neptune
        {(Vec3){0, 0, -15}, .2, 0, (Color){255, 255, 255}},       // sun
    };

    int i = 0;
    while (planets[i].speed >= 0)
    {
        win->scene.objects[win->scene.pos].type = sphere_;
        win->scene.objects[win->scene.pos].mat = Abs_;
        win->scene.objects[win->scene.pos].center = planets[i].center;
        win->scene.objects[win->scene.pos].radius = planets[i].rad;
        win->scene.objects[win->scene.pos].speed = planets[i].speed;
        win->scene.objects[win->scene.pos].color = planets[i].color;
        win->scene.pos++;
        i++;
        std::cout << "add sphere" << std::endl;
        // win->scene.objects[win->scene.pos].light_intensity = planets[i].light_intensity;
        // win->scene.objects[win->scene.pos].light_color = (Color){1, 1, 1};
    }
    Vec3 p1, p2, p3;
    p1 = (Vec3){0, 0, 0};
    p2 = (Vec3){-2, 0, 0};
    p3 = (Vec3){0, -1, 0};
    win->scene.objects[win->scene.pos++] = new_rectangle(p1, p2, p3, (Color){1, 0, 0}, Abs_);
    win->scene.objects[win->scene.pos++] = new_cylinder(p3, 1.0, (Vec3){1, 1, 0}, (Color){1, 0, 0}, Abs_);
#endif
}
