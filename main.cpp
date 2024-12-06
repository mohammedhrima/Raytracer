#include "header.hpp"


// dimentions
#define WIDTH 800
#define HEIGHT WIDTH
_Atomic int frame_index;

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