/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 20:19:24 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/09 11:59:48 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	compute_intersection_hit(t_hit_info *hit, t_object *e,
		t_vector ray_origin, t_vector ray_dir)
{
	hit->o = vector_sub(ray_origin, e->center);
	hit->v = square(e->radius) / square(e->height);
	hit->a = square(vector_dot(ray_dir, e->xaxis))
		+ square(vector_dot(ray_dir, e->yaxis))
		- hit->v * square(vector_dot(ray_dir, e->axis));
	hit->b = 2 * (vector_dot(hit->o, e->xaxis) * vector_dot(ray_dir, e->xaxis)
			+ vector_dot(hit->o, e->yaxis) * vector_dot(ray_dir, e->yaxis))
		- (hit->v * 2 * vector_dot(hit->o, e->axis)
			* vector_dot(ray_dir, e->axis) - (2 * e->radius
				* e->radius / e->height) * vector_dot(ray_dir, e->axis));
	hit->c = square(vector_dot(hit->o, e->xaxis))
		+ square(vector_dot(hit->o, e->yaxis)) - e->radius * e->radius
		- (hit->v * square(vector_dot(hit->o, e->axis))
			- (2 * e->radius * e->radius / e->height)
			* vector_dot(hit->o, e->axis));
	hit->delta = hit->b * hit->b - 4 * hit->a * hit->c;
}

void	ray_intersect_cone(t_hit_info *hit, t_object *e,
		t_vector ray_origin, t_vector ray_dir)
{
	compute_intersection_hit(hit, e, ray_origin, ray_dir);
	if (hit->delta < 0 || !hit->a)
		return ;
	hit->delta = sqrtf(hit->delta);
	hit->t0 = (-hit->b - hit->delta) / (2 * hit->a);
	hit->t1 = (-hit->b + hit->delta) / (2 * hit->a);
	hit->d = vector_dot(vector_sub(ray_at(ray_origin, ray_dir, hit->t0),
				e->center), e->axis);
	if (hit->t0 > MIN_T && hit->d >= 0 && hit->d <= e->height
		&& (hit->t < 0 || hit->t0 < hit->t))
		hit->t = hit->t0;
	hit->d = vector_dot(vector_sub(ray_at(ray_origin, ray_dir, hit->t1),
				e->center), e->axis);
	if (hit->t1 > MIN_T && hit->d >= 0 && hit->d <= e->height
		&& (hit->t < 0 || hit->t1 < hit->t))
		hit->t = hit->t1;
	hit->p = vector_normalize(vector_sub(ray_at(ray_origin, ray_dir, hit->t),
				vector_add(e->center, vector_scale(e->axis, e->height))));
	hit->normal = vector_cross(vector_cross(hit->p, e->axis), hit->p);
	hit->tangent = vector_cross(hit->normal, e->axis);
}

void	ray_intersect_cone_opening(t_hit_info *hit, t_object *e,
		t_vector ray_origin, t_vector ray_dir)
{
	hit->open = 0;
	hit->t0 = ray_intersect_plane(ray_origin, ray_dir, e->axis,
			-vector_dot(e->center, e->axis));
	hit->p = vector_sub(ray_at(ray_origin, ray_dir, hit->t0), e->center);
	if (hit->t0 > MIN_T && (hit->t < 0 || hit->t0 < hit->t)
		&& vector_length(hit->p) <= e->radius)
	{
		hit->t = hit->t0;
		hit->normal = vector_scale(e->axis, -1);
		hit->tangent = vector_cross(get_up_vector(hit->normal), hit->normal);
		hit->open = 1;
	}
}

t_hit_info	ray_hit_cone(t_object *e, t_vector ray_origin, t_vector ray_dir)
{
	t_hit_info	hit;

	hit.t = -1;
	ray_intersect_cone(&hit, e, ray_origin, ray_dir);
	ray_intersect_cone_opening(&hit, e, ray_origin, ray_dir);
	if (hit.t < MIN_T)
		return (hit);
	hit.p = vector_sub(ray_at(ray_origin, ray_dir, hit.t), e->center);
	hit.u = 0.5f + atan2f(vector_dot(e->yaxis, hit.p),
			vector_dot(e->xaxis, hit.p)) / (2 * PI);
	if (hit.open)
		hit.v = 1;
	else
		hit.v = 1 - (0.5f + vector_dot(hit.p, e->axis) / e->height);
	return (hit);
}
