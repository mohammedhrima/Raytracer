#include "header.h"

void put_pixel(Image *image, int x, int y, int color)
{
    char *dst;

    dst = image->pixels + ((y % HEIGHT) * image->line_length + (x % WIDTH) * (image->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

void put_pixels(Image *image, int x, int y, int width, int height, int color)
{
    int x_start = (x %= WIDTH);
    int y_start = (y %= HEIGHT);

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