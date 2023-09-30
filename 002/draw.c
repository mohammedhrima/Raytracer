#include "header.h"

void put_pixel(Image *image, int x, int y, int color)
{
    char *dst;
    // printf("%d\n", y);
    while (y < 0)
        y += HEIGHT;
    while (x < 0)
        x += WIDTH;
    while (y > HEIGHT)
        y %= HEIGHT;
    while(x > WIDTH)
        x %= WIDTH;

    dst = image->pixels + ((y % (HEIGHT - 1)) * image->line_length + (x % (WIDTH - 1)) * (image->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

void put_pixels(Image *image, int x, int y, int width, int height, int color)
{
    int x_start = x;
    int y_start = y;

    while (x_start < x + width)
    {
        y_start = y;
        while (y_start < y + height)
        {
            put_pixel(image, x_start, y_start, color);
            y_start++;
        }
        x_start++;
    }
}