#include "header.h"

float step = 10;
float x = 0.0;
float y = 10;
Player p1;
Player p2;

void my_sleep(float time)
{
    float x = 0.0;
    while (x < time)
        x += 0.05;
}

int draw(Var *var)
{
    my_sleep(30000);
    clear_screen(var);
    draw_player(var, &p1);
    // if (p1.jumping == 10)
    // {
    //     p1.ny = p1.y - 30;
    //     // p1.jumping = 0;
    //     // p1.ny = p1.ny - p1.v > 0 ? p1.ny - p1.v : HEIGHT - p1.v;
    // }
    // if (p1.jumping > 0 && p1.v > 0.0)
    // {
    //     p1.v *= 2;
    //     p1.jumping--;
    // }
    // if (p1.v > 10)
    //     p1.jumping = -10;
    // if (p1.jumping < 0)
    // {
    //     p1.v = 0.05;
    //     p1.jumping++;
    // }
    mlx_put_image_to_window(var->mlx, var->win, var->image.ptr, 0, 0);
    return (0);
}

int main(void)
{

    Var *var = calloc(1, sizeof(Var));
    var->mlx = mlx_init();
    var->win = mlx_new_window(var->mlx, WIDTH, HEIGHT, "Pacman");
    var->image.ptr = mlx_new_image(var->mlx, WIDTH, HEIGHT);
    var->image.pixels = mlx_get_data_addr(var->image.ptr,
                                          &var->image.bits_per_pixel,
                                          &var->image.line_length,
                                          &var->image.endian);

    p1 = new_player(10.0, 2, RED);

    mlx_loop_hook(var->mlx, draw, var);
    mlx_hook(var->win, 3, 3, clicked_key, var);
    // mlx_hook(var->win, DESTROY, 0, on_close, var);
    mlx_loop(var->mlx);
}