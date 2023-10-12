#ifndef HEADER_H
#define HEADER_H

#include <mlx.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

extern int WIDTH;
extern int HEIGHT;

#define ESC 53
#define UP 126
#define DOWN 125
#define LEFT 123
#define RIGHT 124

#define RED 0xff0000
#define GREEN 0x51FA0E
#define BLUE 0x000000FF
#define ORANGE 0xed6a12
#define YELLOW 0xFFFF00
#define BACKGROUND 0xD9D9D9
#define WHITE 0xFFFFFF

typedef enum
{
    Reflectif_ = 11,
    Refractif_,
    Absorb_
} Mat;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
    };
    struct
    {
        float r;
        float g;
        float b;
    };
} Coor;

typedef Coor Color;

typedef struct
{
    Coor center;
    float radius;
    Color color;
    Mat type;
} Sphere;

typedef struct
{
    Coor dir;
    Coor org;
} Ray;

typedef struct
{
    // objects
    Sphere spheres[100];
    int pos;
} Scene;

typedef struct
{
    void *mlx;
    void *win;
    // image
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    // rendering
    float focal_length;
    float aspect_ratio;
    Coor camera;
    Coor viewport_u;
    Coor viewport_v;
    Coor pixel_u;
    Coor pixel_v;
    Coor first_pixel;
    // objects
    Sphere spheres[100];
    int pos;
} Win;


#endif