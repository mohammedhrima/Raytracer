/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 20:16:42 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/07 18:07:52 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	ray_intersect_cylinder_opening(t_hit_info *hit, t_object *e,
			t_vector ray_origin, t_vector ray_dir)
{
	hit->open = 0;
	hit->c0 = vector_add(e->center, vector_scale(e->axis, e->height / 2));
	hit->c1 = vector_add(e->center, vector_scale(e->axis, -e->height / 2));
	hit->t0 = ray_intersect_plane(ray_origin, ray_dir, e->axis,
			-vector_dot(hit->c0, e->axis));
	hit->t1 = ray_intersect_plane(ray_origin, ray_dir, e->axis,
			-vector_dot(hit->c1, e->axis));
	if (vector_length(vector_sub(ray_at(ray_origin, ray_dir, hit->t0),
				hit->c0)) <= e->radius
		&& hit->t0 > MIN_T && (hit->t < 0 || hit->t0 < hit->t))
	{
		hit->t = hit->t0;
		hit->normal = e->axis;
		hit->tangent = vector_cross(get_up_vector(hit->normal), hit->normal);
		hit->open = 1;
	}
	if (vector_length(vector_sub(ray_at(ray_origin, ray_dir, hit->t1),
				hit->c1)) <= e->radius
		&& hit->t1 > MIN_T && (hit->t < 0 || hit->t1 < hit->t))
	{
		hit->t = hit->t1;
		hit->normal = vector_scale(e->axis, -1);
		hit->tangent = vector_cross(get_up_vector(hit->normal), hit->normal);
		hit->open = -1;
	}
}

void	ray_intersect_cylinder(t_hit_info *hit, t_object *e,
			t_vector ray_origin, t_vector ray_dir)
{
	hit->o = vector_sub(ray_origin, e->center);
	hit->a = square(vector_dot(ray_dir, e->xaxis))
		+ square(vector_dot(ray_dir, e->yaxis));
	hit->b = 2 * (vector_dot(hit->o, e->xaxis) * vector_dot(ray_dir, e->xaxis)
			+ vector_dot(hit->o, e->yaxis) * vector_dot(ray_dir, e->yaxis));
	hit->c = square(vector_dot(hit->o, e->xaxis))
		+ square(vector_dot(hit->o, e->yaxis)) - e->radius * e->radius;
	hit->delta = hit->b * hit->b - 4 * hit->a * hit->c;
	if (hit->delta < 0 || hit->a == 0)
		return ;
	hit->delta = sqrtf(hit->delta);
	hit->t0 = (-hit->b - hit->delta) / (2 * hit->a);
	hit->t1 = (-hit->b + hit->delta) / (2 * hit->a);
	if (hit->t0 > MIN_T)
		hit->t = hit->t0;
	if (hit->t1 > MIN_T && (hit->t < 0 || hit->t1 < hit->t))
		hit->t = hit->t1;
	hit->p = vector_sub(ray_at(ray_origin, ray_dir, hit->t), e->center);
	hit->d = vector_dot(hit->p, e->axis);
	if (fabsf(hit->d) > e->height / 2)
		hit->t = -1;
	hit->normal = vector_sub(hit->p, vector_scale(e->axis, hit->d));
	hit->tangent = vector_cross(hit->normal, e->axis);
}

t_hit_info	ray_hit_cylinder(t_object *e, t_vector ray_origin, t_vector ray_dir)
{
	t_hit_info	hit;

	hit.t = -1;
	ray_intersect_cylinder(&hit, e, ray_origin, ray_dir);
	ray_intersect_cylinder_opening(&hit, e, ray_origin, ray_dir);
	if (hit.t < MIN_T)
		return (hit);
	hit.p = vector_sub(ray_at(ray_origin, ray_dir, hit.t), e->center);
	hit.u = 0.5f + atan2f(vector_dot(e->yaxis, hit.p),
			vector_dot(e->xaxis, hit.p)) / (2 * PI);
	if (hit.open < 0)
		hit.v = 0;
	else if (hit.open > 0)
		hit.v = 1;
	else
		hit.v = 1.f - (0.5f + vector_dot(hit.p, e->axis) / e->height);
	return (hit);
}
