/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 20:16:58 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/07 19:15:38 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

#define EPSILON 0.0001f

float	ray_intersect_plane(t_vector ray_origin, t_vector ray_dir,
		t_vector normal, float d)
{
	float	denom;

	denom = vector_dot(normal, ray_dir);
	if (denom > EPSILON || denom < -EPSILON)
		return ((-d - vector_dot(normal, ray_origin)) / denom);
	return (-1);
}

t_hit_info	ray_hit_plane(t_object *e, t_vector ray_origin, t_vector ray_dir)
{
	t_hit_info	hit;

	hit.t = ray_intersect_plane(ray_origin, ray_dir, e->axis, e->d);
	if (hit.t < MIN_T)
	{
		hit.t = -1;
		return (hit);
	}
	hit.normal = e->axis;
	if (vector_dot(hit.normal, ray_dir) > 0)
		hit.normal = vector_scale(hit.normal, -1);
	hit.p = vector_add(ray_origin, vector_scale(ray_dir, hit.t));
	hit.u = -vector_dot(hit.p, e->xaxis);
	hit.v = vector_dot(hit.p, e->yaxis);
	hit.tangent = vector_scale(e->xaxis, -1);
	return (hit);
}
