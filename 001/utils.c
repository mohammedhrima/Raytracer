#include "header.h"

Player new_player(float x, float y, int color)
{
    Player p;
    p.x = x;
    p.y = y;
    p.nx = x;
    p.ny = y;
    p.color = color;
    return p;
}

Point new_point(float x, float y)
{
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

Vector new_vector(float x, float y)
{
    Vector v;
    v.x = x;
    v.y = y;
    return v;
}

Point move_point(Point p, Vector v)
{
    return new_point(p.x + v.x, p.y + v.y);
}

Vector sub_points(Point l, Point r)
{
    return new_vector(l.x - r.x, l.y - r.y);
}

Vector add_points(Point l, Point r)
{
    return new_vector(l.x + r.x, l.y + r.y);
}

float vector_length(Vector v)
{
    return ((float)sqrt(v.x * v.x + v.y * v.y));
}

float vector_lengthSqrt(Vector v)
{
    return (v.x * v.x + v.y * v.y);
}

Vector scale_vector(Vector v, float scale)
{
    return new_vector(v.x * scale, v.y * scale);
}

Vector Normalized(Vector v)
{
    float len = vector_length(v);
    if (len == 0)
        return new_vector(0, 0);
    return new_vector(v.x / len, v.y / len);
}

Vector sub_vectors(Vector l, Vector r)
{
    return new_vector(l.x - r.x, l.y - r.y);
}

Vector add_vectors(Vector l, Vector r)
{
    return new_vector(l.x + r.x, l.y + r.y);
}

float DotProduct(Vector l, Vector r)
{
    return l.x * r.x + l.y * r.y;
}

int clicked_key(int keycode, Var *var)
{
    // printf(">%d\n", keycode);
    switch (keycode)
    {
    case UP:
        p1.ny -= step * 2;
        break;
    case DOWN:
        p1.ny += step * 2;
        break;
    case RIGHT:
        p1.nx += step * 2;
        break;
    case LEFT:
        p1.nx -= step * 2;
        break;
    case ESC:
        exit(0);
        break;
    default:
        break;
    }
    // Vector br = Normalized(new_vector(p2.x - p1.x, p2.y - p1.y));
    // printf("<%f>\n", DotProduct(new_vector(p2.x, p2.y), new_vector(p1.x, p1.y)));
    // printf("< %f, %f >\n", br.x, br.y);
    return 0;
}

void clear_screen(Var *var)
{
    put_pixels(&var->image, 0, 0, WIDTH, HEIGHT, BACKGROUND);
}

void draw_player(Var *var, Player *p)
{
#if 1
    p->x = p->x + (p->nx - p->x) * 0.05;
    p->y = p->y + (p->ny - p->y) * 0.05;
#else
    p->x = p->nx;
    p->y = p->ny;
#endif
    put_pixels(&var->image, (int)p->x, (int)p->y, step * 2, step * 2, p->color);
}

void move()
{
    x += step;
    if ((int)x >= WIDTH)
        x = 0.0;
}
