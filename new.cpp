#include "header.hpp"

Obj *new_sphere(Vec3 center, float radius, Color color, Mat mat)
{
    std::cout << "new sphere" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = SPHERE;
    obj->center = center;
    obj->radius = radius;
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_plan(Vec3 normal, float d, Color color, Mat mat)
{
    std::cout << "new plan" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = PLAN;
    obj->normal = unit_vector(normal);
    obj->d = d;
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_triangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    std::cout << "new triangle" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = TRIANGLE;
    obj->p1 = p1;
    obj->p2 = p2;
    obj->p3 = p3;
    obj->normal = cross(p2 - p1, p3 - p1);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_rectangle(Vec3 p1, Vec3 p2, Vec3 p3, Color color, Mat mat)
{
    std::cout << "new rectangle" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = RECTANGLE;
    obj->p1 = p1;
    obj->p2 = p2;
    obj->p3 = p3;
    obj->normal = cross(p2 - p1, p3 - p1);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_cylinder(Vec3 center, float radius, float height, Vec3 normal, Color color, Mat mat)
{
    std::cout << "new cylinder" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = CYLINDER;
    obj->center = center;
    obj->radius = radius;
    obj->height = height;
    obj->normal = unit_vector(normal);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Obj *new_cone(Vec3 center, float radius, float height, Vec3 normal, Color color, Mat mat)
{
    std::cout << "new cone" << std::endl;
    Obj *obj = (Obj *)calloc(1, sizeof(Obj));
    obj->type = CONE;
    obj->center = center;
    obj->radius = radius;
    obj->height = height;
    obj->normal = unit_vector(normal);
    obj->color = color;
    obj->mat = mat;
    return obj;
}
Multi *new_multi(Win *win, int idx, int cols, int rows)
{
    Multi *multi = (Multi *)calloc(1, sizeof(Multi));
    int cellWidth = win->width / cols;
    int cellHeight = win->height / rows;
    multi->x_start = (idx % cols) * cellWidth;
    multi->y_start = (idx / cols) * cellHeight;
    multi->x_end = multi->x_start + cellWidth - 1;
    multi->y_end = multi->y_start + cellHeight - 1;
    multi->idx = idx;
    multi->win = win;
    return multi;
}