#ifndef HEADER_H
#define HEADER_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <mlx.h>

#define HEIGHT 300
#define WIDTH 500
#define BACKGROUND 0xD9D9D9
#define RED 0xff0000
#define GREEN 0x51FA0E

#define UP 65362
#define DOWN 65364
#define LEFT 65361
#define RIGHT 65363
#define ESC 65307



typedef struct
{
    int line_length;
    int bits_per_pixel;
    int endian;
    char *pixels;
    void *ptr;
} Image;

typedef struct
{
    Image image;
    void *mlx;
    void *win;
    int keycode;
    int endian;
} Var;

typedef struct
{
    float x;
    float y;
} Point;

typedef Point Vector;

typedef struct
{
    float x;
    float y;
    int color;
} Player;

extern float step;
extern float x;
extern float y ;
extern Player p2;
extern Player p1;

void put_pixel(Image *image, int x, int y, int color);
void put_pixels(Image *image, int x, int y, int width, int height, int color);
Point new_point(float x, float y);
Vector new_vector(float x, float y);
Point move_point(Point p, Vector v);
Vector sub_points(Point l, Point r);
Vector add_points(Point l, Point r);
float vector_length(Vector v);
float vector_lengthSqrt(Vector v);
Vector scale_vector(Vector v, float scale);
Vector Normalized(Vector v);
Vector sub_vectors(Vector l, Vector r);
Vector add_vectors(Vector l, Vector r);
int clicked_key(int keycode, Var *var);
void clear_screen(Var *var);
void draw_player(Var *var, Player *p);
Player new_player(float x, float y, int color);
float DotProduct(Vector l, Vector r);

#endif