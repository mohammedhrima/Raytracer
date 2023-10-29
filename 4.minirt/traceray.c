#include "header.h"

t_ray render_object(t_obj obj, t_ray ray, float closest)
{
    // point coordinates
    t_vec3 cp_norm = (t_vec3){};
    t_vec3 p = point_at(ray, closest);
    if (obj.type == sphere_)
        cp_norm = unit_vector(calc(p, '-', obj.sphere.center));
    else if (obj.type == plan_)
        cp_norm = obj.plan.normal;

    bool same_dir = unit_dot(cp_norm, ray.dir) >= 0;
    if (same_dir) // to be used when drawing triangle
        cp_norm = (t_vec3){.x = -cp_norm.x, .y = -cp_norm.y, .z = -cp_norm.z};
    t_vec3 ranv = random_unit_vector();
    t_vec3 ndir;
#if 0
    if (obj.mat == Refl_)
    {
        float val;
        val = -2 * dot(ray.dir, cp_norm);
        ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
    }
    else if (obj.mat == Refr_)
    {
        float index_of_refraction = 1.5;
        float refraction_ratio = same_dir ? index_of_refraction : (1.0 / index_of_refraction);

        float cos_theta = dot(ray.dir, cp_norm) / (length(ray.dir) * length(cp_norm));
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (refraction_ratio * sin_theta > 1.0)
        {
            // Reflect
            float val;
            val = -2 * dot(ray.dir, cp_norm);
            ndir = (Vec3){ray.dir.x + val * cp_norm.x, ray.dir.y + val * cp_norm.y, ray.dir.z + val * cp_norm.z};
        }
        else
        {
            // Refract
            Vec3 ray_dir = unit_vector(ray.dir);
            Vec3 Perp = scale(refraction_ratio, sub_vec3(ray_dir, scale(dot(ray_dir, cp_norm), cp_norm)));
            Vec3 Para = scale(sqrt(1 - pow(length(Perp), 2)), scale(-1, cp_norm));
            ndir = add_vec3(Perp, Para);
        }
    }
    else if (obj.mat == Abs_)
#endif
    ndir = calc(cp_norm, '+', ranv);
    return (t_ray){.org = p, .dir = ndir};
}

t_color ray_color(t_win *win, t_ray ray, int depth)
{
    t_scene *scene = &win->scene;
    t_color light = {};
    t_color attenuation = {.x = 1, .y = 1, .z = 1};

    for (int bounce = 0; bounce < depth; bounce++)
    {
        float closest = FLT_MAX;
        int hit_index = -1;
        float x = 0;
        for (int i = 0; i < scene->pos; i++)
        {
            if (scene->objects[i].type == sphere_)
                x = hit_sphere(scene->objects[i].sphere, ray, ZERO, closest);
            else if (scene->objects[i].type == plan_)
                x = hit_plan(scene->objects[i].plan, ray, ZERO, closest);
            if (x > .0)
            {
                hit_index = i;
                closest = x;
            }
        }
        if (hit_index != -1)
        {
            t_obj *obj = &scene->objects[hit_index];
            ray = render_object(*obj, ray, closest);
            light = calc(light, '+', calc(attenuation, '*', scale(obj->light_color, '*', obj->light_intensity)));
            attenuation = calc(attenuation, '*', obj->color);
        }
        else
        {
            float a = 0.5 * (unit_vector(ray.dir).y + 1.0);
            float r = (1.0 - a) + a * .3;
            float g = (1.0 - a) + a * .7;
            float b = (1.0 - a) + a * 1.0;
            light = calc(light, '+', calc(attenuation, '*', (t_color){.x = r, .y = g, .z = b}));
            break;
        }
        if (attenuation.x <= ZERO && attenuation.y <= ZERO && attenuation.z <= ZERO)
            break;
    }
    return light;
}
