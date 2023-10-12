#include "header.h"

// mlx
int close_window(int keycode, Win *vars)
{
    exit(0);
}

int listen(int keycode, Win *vars)
{
    switch (keycode)
    {
    case ESC:
        mlx_destroy_window(vars->mlx, vars->win);
        exit(0);
    default:
        printf("%d\n", keycode);
    }
    return (0);
}

// void put_pixel(Win *data, int x, int y, int color)
// {
//     char *dst;

//     dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
//     *(unsigned int *)dst = color;
// }