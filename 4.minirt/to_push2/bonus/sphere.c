/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 20:16:51 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/07 19:23:45 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

float	get_sphere_hit_t(t_hit_info *hit, t_object *e,
		t_vector ray_origin, t_vector ray_dir)
{
	hit->o = vector_sub(ray_origin, e->center);
	hit->a = vector_dot(ray_dir, ray_dir);
	hit->b = 2 * vector_dot(hit->o, ray_dir);
	hit->c = vector_dot(hit->o, hit->o) - e->radius * e->radius;
	hit->delta = hit->b * hit->b - 4 * hit->a * hit->c;
	if (hit->delta < 0 || !hit->a)
		return (-1);
	hit->delta = sqrtf(hit->delta);
	hit->t0 = (-hit->b - hit->delta) / (2 * hit->a);
	hit->t1 = (-hit->b + hit->delta) / (2 * hit->a);
	if (hit->t0 > MIN_T)
		return (hit->t0);
	if (hit->t1 > MIN_T)
		return (hit->t1);
	return (-1);
}

t_hit_info	ray_hit_sphere(t_object *e, t_vector ray_origin, t_vector ray_dir)
{
	t_hit_info	hit;

	hit.t = get_sphere_hit_t(&hit, e, ray_origin, ray_dir);
	if (hit.t < MIN_T)
	{
		hit.t = -1;
		return (hit);
	}
	hit.normal = vector_normalize(vector_sub(
				ray_at(ray_origin, ray_dir, hit.t), e->center));
	hit.u = 1 - (0.5f + atan2f(hit.normal.z, hit.normal.x) / (2 * PI));
	hit.v = acosf(hit.normal.y) / PI;
	hit.tangent = vector_cross(get_up_vector(hit.normal), hit.normal);
	return (hit);
}
