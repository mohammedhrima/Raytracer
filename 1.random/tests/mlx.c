#include <mlx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    int width;
    int height;
    void *mlx;
    void *win;
    // image
    void *img;
    void *title;

    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
} Win;

int on_mouse_move(int x, int y, Win *win)
{
    return 0;
}

int on_mouse_up(int code, int x, int y, Win *win)
{
    return 0;
}
int on_mouse_down(int code, int x, int y, Win *win)
{
    return 0;
}

void draw_rec(Win *win, int x_start, int y_start, int width, int height, uint32_t color)
{
    for (int i = x_start; i < x_start + width; i++)
    {
        for (int j = y_start; j < y_start + height; j++)
        {
            uint32_t color32 = color;
            uint32_t *pixel = (uint32_t *)win->addr + j * win->width + i;
            *pixel = color32;
        }
    }
}

int draw(void *ptr)
{
    static float x;
    Win *win = (Win *)ptr;
    draw_rec(win, 0, 0, win->height, win->width, 0);
    draw_rec(win, (int)x % win->width, 10, 30, 30, (uint32_t)(255 << 16) | (uint32_t)(0 << 8) | (uint32_t)(0 << 0));
    x = ((x + 0.5));

    mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
    return 0;
}

int listen_on_key(int key, Win *win)
{
    printf("%d\n", key);
    if (key == 65307)
        exit(0);
}

int main(void)
{
    Win win = {0};
    win.width = 512;
    win.height = (int)((float)win.width / 1);
    if (win.height < 1)
        win.height = 1;

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
