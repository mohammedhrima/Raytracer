#include "header.h"

t_obj new_plan(t_vec3 normal, float d, t_color color)
{
    t_obj new = {0};
    new.type = plan_;
    new.plan.normal = unit_vector(normal);
    new.plan.d = d;
    new.color = color;
    return new;
}
t_obj new_sphere(t_vec3 center, float radius, t_color color)
{
    t_obj new = {0};
    new.type = sphere_;
    new.sphere.center = center;
    new.sphere.radius = radius;
    new.color = color;
    return new;
}
void add_objects(t_win *win)
{
    // protect it from heap oveflow
    win->scene.objects = ft_calloc(100, sizeof(t_obj));
    struct
    {
        t_vec3 org;
        float rad;
    } spheres[] = {
        {(t_vec3){.x = 1, .y = 1, .z = -1}, .5},
        {(t_vec3){.x = -.5, .y = -.5, .z = -1}, .5},
        {(t_vec3){.x = -1, .y = 1, .z = -1}, .5},
        {(t_vec3){.x = 1, .y = -1, .z = -1}, .5},
        {(t_vec3){.x = 0.5, .y = 0, .z = -1}, .5},
        {(t_vec3){}, -1},
    };
    t_color colors[] = {
        (t_color){.x = 0.92, .y = 0.19, .z = 0.15},
        (t_color){.x = 0.42, .y = 0.92, .z = 0.72},
        (t_color){.x = 0.42, .y = 0.87, .z = 0.92},
        (t_color){.x = 0.30, .y = 0.92, .z = 0.64},
        (t_color){.x = 0.39, .y = 0.92, .z = 0.63},
        (t_color){.x = 0.42, .y = 0.92, .z = 0.80},
        (t_color){.x = 0.47, .y = 0.16, .z = 0.92},
        (t_color){.x = 0.42, .y = 0.58, .z = 0.92},
        (t_color){.x = 0.92, .y = 0.40, .z = 0.30},
        (t_color){.x = 0.61, .y = 0.75, .z = 0.24},
        (t_color){.x = 0.83, .y = 0.30, .z = 0.92},
        (t_color){.x = 0.23, .y = 0.92, .z = 0.08},
    };
    int i = 0;
    while (spheres[i].rad > 0)
    {
        t_vec3 org = spheres[i].org;
        float rad = spheres[i].rad;
        win->scene.objects[win->scene.pos] = new_sphere(org, rad, colors[win->scene.pos % (sizeof(colors) / sizeof(*colors))]);
        win->scene.pos++;
        i++;
    }
#if 0
    struct
    {
        Vec3 normal;
        float dist; // distance from camera
    } plans[] = {
        {(Vec3){.x = 0, .y = -1, .z = 0}, -4}, // up
        {(Vec3){.x = 0, .y = 1, .z = 0}, -4},  // down
        {(Vec3){.x = 0, .y = 0, .z = 1}, -12}, // behind
        {(Vec3){.x = 1, .y = 0, .z = 0}, -4},  // right
        {(Vec3){.x = -1, .y = 0, .z = 0}, -4}, // left
        {(Vec3){}, 0},
    };
    i = 0;
    while (plans[i].mat)
    {
        Vec3 normal = plans[i].normal;
        float dist = plans[i].dist;
        Mat mat = plans[i].mat;
        win.scene.objects[win.scene.pos] = new_plan(normal, dist, colors[win.scene.pos % (sizeof(colors) / sizeof(*colors))], mat);
        if (i == 0)
        {
            win.scene.objects[win.scene.pos].light_intensity = 1;
            win.scene.objects[win.scene.pos].light_color = (Color){1, 1, 1};
        }
        i++;
        win.scene.pos++;
    }
#endif
}