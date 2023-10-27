#include "utils.cpp"
_Atomic(int) frame_index;
void add_objects(Win *win);
int frame_shots = 0;

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

    scene->camera = (Vec3){0, -16, FOCAL_LEN};
    scene->upv = (Vec3){0, 1, 0};

    sum = (Color *)calloc(win->width * win->height, sizeof(Color));
    init(win);
    frame_index = 1;
    int quit = 0;
    int frame_shot = 0;
    add_objects(win);

    for (int i = 0; i < THREADS_LEN; i++)
        win->thread_finished[i] = 1;

    Multi *multis[THREADS_LEN];
    for (int i = 0; i < THREADS_LEN; i++)
    {
        multis[i] = new_multi(i, win);
        pthread_create(&multis[i]->thread, nullptr, TraceRay, multis[i]);
    }
    struct
    {
        Vec3 move;
        char *msg;
    } trans[1000] = {
        [FORWARD - 1073741900] = {(Vec3){0, 0, -1}, (char *)"forward"},
        [BACKWARD - 1073741900] = {(Vec3){0, 0, 1}, (char *)"backward"},
        [UP - 1073741900] = {(Vec3){0, 1, 0}, (char *)"up"},
        [DOWN - 1073741900] = {(Vec3){0, -1, 0}, (char *)"down"},
        [LEFT - 1073741900] = {(Vec3){-1, 0, 0}, (char *)"left"},
        [RIGHT - 1073741900] = {(Vec3){1, 0, 0}, (char *)"right"},
    };

    unsigned int *FramesList[FRAMES_LEN];
    int i = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&win->ev) != 0)
        {
            switch (win->ev.type)
            {
            case QUIT:
                quit = true;
                break;
            case MOUSE_DOWN:
                if (win->ev.button.button == MOUSE_LEFT)
                    std::cout << "Left mouse button clicked at (" << win->ev.button.x << ", " << win->ev.button.y << ")" << std::endl;
                is_mouse_down = 1;
                break;
            case MOUSE_UP:
                is_mouse_down = 0;
                break;
            case MOUSE_MOTION:
                on_mouse_move(win, win->ev.motion.x, win->ev.motion.y);
                if (is_mouse_down)
                {
                    scene->upv = (Vec3){0, 1, 0};
                    frame_index = 1;
                    memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
                    init(win);
                }
                break;
            case MOUSE_SCROLL:
                if (win->ev.wheel.y > 0)
                    translate(win, 2 * trans[FORWARD - 1073741900].move);
                else
                    translate(win, 2 * trans[BACKWARD - 1073741900].move);
                // scene->upv = (Vec3){0, 1, 0};
                frame_index = 1;
                memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
                init(win);
                break;
            case PRESS:
                // case RELEASE:
                switch (win->ev.key.keysym.sym)
                {
                case FORWARD:
                case BACKWARD:
                case UP:
                case DOWN:
                case LEFT:
                case RIGHT:
                    translate(win, trans[win->ev.key.keysym.sym - 1073741900].move);
                    // scene->upv = (Vec3){0, 1, 0};
                    frame_index = 1;
                    memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
                    init(win);
                    break;
                case ESC:
                    quit = true;
                    break;
                case RESET:
                    frame_index = 1;
                    memset(sum, COLOR(255, 255, 255), win->width * win->height * sizeof(Color));
                    // scene->upv = (Vec3){0, 1, 0};
                    x_rotation = 0;
                    y_rotation = 0;
                    scene->camera = (Vec3){0, 0, FOCAL_LEN};
                    init(win);
                    std::cout << "Reset" << std::endl;
                    break;
                default:
                    std::cout << "key cliqued: " << win->ev.key.keysym.sym << std::endl;
                    break;
                }
                break;

            default:
                break;
            }
            // if draw is called
            // draw_rect(win, (int)x, (int)y, 20, 20, COLOR(255, 0, 0));
        }
        // set all threads to not finished
        for (int i = 0; i < THREADS_LEN; i++)
            win->thread_finished[i] = 0;
        time_start = get_time();
        while ((FRAMES_LEN && frame_shots < FRAMES_LEN) || !FRAMES_LEN)
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
#if FRAMES_LEN
        {
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
                init(win);
            }
            // play animation
            else
            {
                frame_index = frame_index % FRAMES_LEN;
                std::cout << "Playing frame: " << frame_index << std::endl;
                memcpy(win->pixels, FramesList[frame_index], win->width * win->height * sizeof(uint32_t));
                usleep(1000);
            }
        }
#else
        int i = 0;
        while (win->scene.objects[i].type == sphere_ && win->scene.objects[i].speed >= 0)
        {
            win->scene.objects[i].center = rotate(win, win->scene.objects[i].center, 'z', win->scene.objects[i].speed * degrees_to_radians(1));
            i++;
        }
        frame_index = 0;
        memset(sum, COLOR(0, 0, 0), win->width * win->height * sizeof(Color));
        init(win);
#endif
        frame_index++;
        update_window(win);
        time_end = get_time();
        std::string dynamicTitle = std::to_string(time_end - time_start) + std::string(" ms");
        SDL_SetWindowTitle(win->window, dynamicTitle.c_str());
    }
    close_window(win);
}

void add_objects(Win *win)
{
    struct
    {
        Vec3 center;
        float rad;
        float speed;
        Color color;
    } planets[] = {
        {(Vec3){.5, 0, -15}, .2, 1.6, color(26., 26., 26.)},    // mercury
        {(Vec3){1, 0, -15}, .2, 1.4, color(230., 230., 230.)},  // venus
        {(Vec3){1.5, 0, -15}, .2, 1.2, color(47., 106., 105.)}, // earth
        {(Vec3){2, 0, -15}, .2, 1., color(153., 61., 0.)},      // mars
        {(Vec3){2.5, 0, -15}, .2, .8, color(176., 127., 53.)},  // jupiter
        {(Vec3){3, 0, -15}, .2, .6, color(176., 143., 54.)},    // saturn
        {(Vec3){3.5, 0, -15}, .2, .4, color(85., 128., 170.)},  // uranus
        {(Vec3){4, 0, -15}, .2, .2, color(54., 104., 150.)},    // neptune
        {(Vec3){0, 0, -15}, .2, 0, color(255, 255, 255)},       // sun
        {(Vec3){}, 0, -1, (Color){}},
    };

    Color colors[] = COLORS;
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
}
