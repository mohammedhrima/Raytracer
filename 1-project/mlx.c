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
