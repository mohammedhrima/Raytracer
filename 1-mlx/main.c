#include <mlx.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 200
#define HEIGHT 200
#define ESC 65307
#define UP 65362
#define DOWN 65364
#define LEFT 65361
#define RIGHT 65363

typedef struct s_vars
{
    void *mlx;
    void *win;
    // image
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
} Win;

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

void my_mlx_pixel_put(Win *data, int x, int y, int color)
{
    char *dst;

    dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

int main(void)
{
    Win vars;

    vars.mlx = mlx_init();
    vars.win = mlx_new_window(vars.mlx, WIDTH, HEIGHT, "Hello world!");
    vars.img = mlx_new_image(vars.mlx, WIDTH, HEIGHT);
    vars.addr = mlx_get_data_addr(vars.img, &vars.bits_per_pixel, &vars.line_length, &vars.endian);

    my_mlx_pixel_put(&vars, 5, 5, 0x00FF0000);
    mlx_put_image_to_window(vars.mlx, vars.win, vars.img, 0, 0);

    mlx_hook(vars.win, 2, 1L << 0, listen, &vars);
    mlx_hook(vars.win, 17, 1L << 0, close_window, &vars);
    mlx_loop(vars.mlx);
}