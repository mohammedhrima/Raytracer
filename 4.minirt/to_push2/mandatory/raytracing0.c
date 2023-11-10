/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raytracing0.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 23:26:30 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 13:25:45 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

t_vec3	get_up_vector(t_vec3 forward)
{
	t_vec3	up;
	float	d;

	up = (t_vec3){0, 1, 0};
	d = dot(up, forward);
	if (d > 0.9)
		up = (t_vec3){0, 0, 1};
	else if (d < -0.9)
		up = (t_vec3){0, 0, -1};
	return (up);
}

void	init_scene(t_win *win)
{
	win->scene.w = scale(unit_vector(win->scene.cam_dir), '*', -1);
	win->scene.up = get_up_vector(win->scene.cam_dir);
	win->scene.screen_height = 2 * tan(win->scene.fov) * win->scene.focal_len;
	win->scene.screen_width = win->scene.screen_height;
	win->scene.u = unit_vector(cross(win->scene.up, win->scene.w));
	win->scene.v = unit_vector(cross(win->scene.w, win->scene.u));
	win->scene.screen_u = scale(win->scene.u, '*', win->scene.screen_width);
	win->scene.screen_v = scale(win->scene.v, '*', -win->scene.screen_height);
	win->scene.pixel_u = scale(win->scene.screen_u, '/', win->width);
	win->scene.pixel_v = scale(win->scene.screen_v, '/', win->height);
	win->scene.center = calc(win->scene.camera, '+', scale(win->scene.w, '*',
				-win->scene.focal_len));
	win->scene.upper_left = calc(win->scene.center, '-',
			scale(calc(win->scene.screen_u, '+', win->scene.screen_v), '/', 2));
	win->scene.first_pixel = calc(win->scene.upper_left, '+',
			scale(calc(win->scene.pixel_u, '+', win->scene.pixel_v), '/', 2));
}

int	check_objects(t_scene *scene, t_ray *ray, float *ptr, t_vec3 *normal)
{
	int		hit_index;
	int		i;
	float	x;
	float	closest;

	closest = FLT_MAX;
	i = 0;
	hit_index = -1;
	while (i < scene->pos)
	{
		if (scene->objects[i].type == sphere_)
			x = hit_sphere(ray, scene->objects[i], closest, normal);
		else if (scene->objects[i].type == plan_)
			x = hit_plan(ray, scene->objects[i], closest, normal);
		else
			x = hit_cylinder(ray, scene->objects[i], closest, normal);
		if (x > 0.0)
		{
			hit_index = i;
			closest = x;
		}
		i++;
	}
	*ptr = closest;
	return (hit_index);
}

t_color	hit_object(t_vec3 point, t_scene *scene, t_obj *obj, t_vec3 normal)
{
	t_vec3	light_dir;
	float	d;

	if (obj->type == plan_
		&& float_sign(dot(scene->camera, obj->normal)
			+ obj->d) != float_sign(dot(scene->light,
				obj->normal) + obj->d))
		return (calc(scene->ambient_light_color, '*', obj->color));
	normal = unit_vector(normal);
	light_dir = unit_vector(calc(scene->light, '-', point));
	d = fmax(dot(normal, light_dir), 0) * scene->light_ratio;
	return (calc(calc((t_color){d, d, d}, '+', scene->ambient_light_color),
		'*', obj->color));
}

t_color	ray_color(t_win *win, t_ray ray)
{
	float	closest;
	int		hit_index;
	t_vec3	normal;
	t_vec3	temp;
	int		hit_index2;

	hit_index = check_objects(&win->scene, &ray, &closest, &normal);
	if (hit_index == -1)
		return (win->scene.ambient_light_color);
	if (dot(normal, ray.dir) >= 0)
		return ((t_vec3){0});
	ray.org = point_at(&ray, closest);
	ray.dir = unit_vector(calc(win->scene.light, '-', ray.org));
	hit_index2 = check_objects(&win->scene, &ray, &closest, &temp);
	if (closest < length(calc(win->scene.light, '-', ray.org))
		|| hit_index == hit_index2)
		return (calc(win->scene.ambient_light_color,
				'*', win->scene.objects[hit_index].color));
	return (hit_object(ray.org, &win->scene,
			&win->scene.objects[hit_index], normal));
}
