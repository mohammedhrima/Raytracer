/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raytracing0.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 23:26:30 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 23:46:22 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	init_scene(t_win *win)
{
	win->scene.w = scale(unit_vector(win->scene.cam_dir), '*', -1);
	win->scene.up = (t_vec3){.x = 0, .y = 1, .z = 0};
	win->scene.screen_height = 2 * tan(win->scene.fov) * win->scene.focal_len;
	win->scene.screen_width = win->scene.screen_height * ((float)win->width
			/ win->height);
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
			x = hit_sphere(ray, scene->objects[i], closest);
		if (scene->objects[i].type == plan_)
			x = hit_plan(ray, scene->objects[i], closest);
		else if (scene->objects[i].type == cylinder_)
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
	t_color	color;
	float	d;

	color = (t_color){0, 0, 0};
	if (obj->type == sphere_)
		normal = calc(point, '-', obj->center);
	else if (obj->type == plan_)
		normal = obj->normal;
	normal = unit_vector(normal);
	light_dir = unit_vector(calc(scene->light, '-', point));
	d = fmax(dot(normal, light_dir), 0);
	color = calc(calc((t_color){d, d, d}, '+', scene->ambient_light_color),
			'*', obj->color);
	return (color);
}

t_color	ray_color(t_win *win, t_ray ray)
{
	t_color	color;
	float	closest;
	int		hit_index;
	t_vec3	normal;

	color = (t_color){0, 0, 0};
	hit_index = check_objects(&win->scene, &ray, &closest, &normal);
	if (hit_index != -1)
		return (hit_object(point_at(&ray, closest), &win->scene,
				&win->scene.objects[hit_index], normal));
	else
		color = win->scene.ambient_light_color;
	return (color);
}

t_color	check_pixel(t_color pixel)
{
	pixel = scale(pixel, '/', RAYS_PER_PIXEL);
	if (pixel.x > 1)
		pixel.x = 1;
	if (pixel.y > 1)
		pixel.y = 1;
	if (pixel.z > 1)
		pixel.z = 1;
	return (pixel);
}
