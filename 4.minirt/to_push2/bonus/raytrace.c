/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raytrace.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:33:16 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:33:39 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_vector	sample_texture(t_object *e, float u, float v)
{
	int			x;
	int			y;
	uint32_t	c32;

	if (!e->texture)
		return ((t_vector){1, 1, 1});
	u = u * e->tex_scale_u;
	v = v * e->tex_scale_v;
	u = u - floorf(u);
	v = v - floorf(v);
	assert(u >= 0 && v >= 0);
	x = imin(u * e->texture->width, e->texture->width - 1);
	y = imin(v * e->texture->height, e->texture->height - 1);
	c32 = *(e->texture->pixels + y * e->texture->width + x);
	return (vector_scale((t_vector){(c32 >> 16) & 0xFF, (c32 >> 8) & 0xFF,
			(c32 >> 0) & 0xFF}, 1.f / 255));
}

t_hit_info	ray_get_closest_hit(t_minirt *mrt,
		t_vector ray_origin, t_vector ray_dir)
{
	t_hit_info	res;
	t_hit_info	hit;
	int			i;

	res.t = FLT_MAX;
	res.object = 0;
	i = 0;
	while (i < mrt->object_count)
	{
		if (mrt->objects[i].type == OBJECT_SPHERE)
			hit = ray_hit_sphere(&mrt->objects[i], ray_origin, ray_dir);
		else if (mrt->objects[i].type == OBJECT_PLANE)
			hit = ray_hit_plane(&mrt->objects[i], ray_origin, ray_dir);
		else if (mrt->objects[i].type == OBJECT_CYLINDER)
			hit = ray_hit_cylinder(&mrt->objects[i], ray_origin, ray_dir);
		else if (mrt->objects[i].type == OBJECT_CONE)
			hit = ray_hit_cone(&mrt->objects[i], ray_origin, ray_dir);
		if (hit.t != -1 && hit.t < res.t)
		{
			res = hit;
			res.object = &mrt->objects[i];
		}
		i++;
	}
	return (res);
}

t_vector	get_bump_map_normal(t_hit_info hit)
{
	float		h;
	float		hr;
	float		hu;
	t_vector	bump_normal;

	if (!hit.object->bump)
		return (hit.normal);
	h = sample_bump_map(hit.object, hit.object->bump, hit.u, hit.v);
	hr = sample_bump_map(hit.object, hit.object->bump,
			hit.u + 1.f / hit.object->bump->width, hit.v);
	hu = sample_bump_map(hit.object, hit.object->bump, hit.u,
			hit.v + 1.f / hit.object->bump->height);
	hit.normal = vector_normalize(hit.normal);
	hit.tangent = vector_normalize(hit.tangent);
	hit.bitangent = vector_normalize(vector_cross(hit.normal, hit.tangent));
	bump_normal = vector_normalize(vector_cross(
				(t_vector){1, 0, (hr - h) * BUMP_MAP_STRENGTH},
				(t_vector){0, 1, (hu - h) * BUMP_MAP_STRENGTH}
				));
	return (matrix_mul_vector((t_matrix){
			{{hit.tangent.x, hit.bitangent.x, hit.normal.x},
				{hit.tangent.y, hit.bitangent.y, hit.normal.y},
				{hit.tangent.z, hit.bitangent.z, hit.normal.z},
			}}, bump_normal));
}

t_vector	get_light_contribution(t_minirt *mrt, t_hit_info *hit,
		t_light light, t_vector eye)
{
	t_vector	light_dir;
	t_hit_info	light_hit;
	float		diffuse;
	float		specular;

	light_dir = vector_normalize(vector_sub(light.p, hit->p));
	light_hit = ray_get_closest_hit(mrt, hit->p, light_dir);
	if (light_hit.t < vector_length(vector_sub(light.p, hit->p))
		|| light_hit.object == hit->object)
		return ((t_vector){0});
	if (hit->object->type == OBJECT_PLANE
		&& float_sign(vector_dot(mrt->camera_p, hit->object->axis)
			+ hit->object->d) != float_sign(vector_dot(light.p,
				hit->object->axis) + hit->object->d))
		return ((t_vector){0});
	diffuse = fmax(0, vector_dot(hit->normal, light_dir));
	specular = powf(fmax(0, vector_dot(
					vector_normalize(vector_reflect(light_dir, hit->normal)),
					vector_normalize(vector_sub(hit->p, eye)))),
			SPECULAR_POWER);
	return (vector_mul(light.color,
			vector_add(
				(t_vector){diffuse, diffuse, diffuse},
			(t_vector){specular, specular, specular})));
}

t_vector	raytrace(t_minirt *mrt, t_vector ray_origin, t_vector ray_dir)
{
	t_hit_info	hit;
	t_vector	color;
	int			i;

	hit = ray_get_closest_hit(mrt, ray_origin, ray_dir);
	if (!hit.object)
		return (mrt->ambient_light_color);
	if (vector_dot(hit.normal, ray_dir) >= 0)
		return ((t_vector){0});
	hit.p = vector_add(ray_origin, vector_scale(ray_dir, hit.t));
	hit.normal = vector_normalize(get_bump_map_normal(hit));
	color = mrt->ambient_light_color;
	i = 0;
	while (i < mrt->light_count)
		color = vector_add(color, get_light_contribution(mrt, &hit,
					mrt->lights[i++], ray_origin));
	return (vector_mul(color, vector_mul(hit.object->color,
				sample_texture(hit.object, hit.u, hit.v))));
}
