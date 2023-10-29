/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_object.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 11:20:08 by mhrima            #+#    #+#             */
/*   Updated: 2023/10/29 11:56:30 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

float	hit_sphere(t_sphere sphere, t_ray ray, float min, float max)
{
	t_vec3	oc;
	t_equ	equ;

	oc = calc(ray.org, '-', sphere.center);
	equ.a = length_squared(ray.dir);
	equ.half_b = unit_dot(oc, ray.dir);
	equ.c = length_squared(oc) - sphere.radius * sphere.radius;
	equ.delta = equ.half_b * equ.half_b - equ.a * equ.c;
	if (equ.delta < .0)
		return (-1.0);
	equ.delta = sqrtf(equ.delta);
	equ.sol = (-equ.half_b - equ.delta) / equ.a;
	if (equ.sol <= min || equ.sol >= max)
		equ.sol = (-equ.half_b + equ.delta) / equ.a;
	if (equ.sol <= min || equ.sol >= max)
		return (-1.0);
	return (equ.sol);
}

float	hit_plan(t_plan plan, t_ray ray, float min, float max)
{
	float	t;
	float	div;

	t = plan.d - unit_dot(plan.normal, ray.org);
	div = unit_dot(ray.dir, plan.normal);
	if (fabsf(div) <= ZERO)
		return (-1.0);
	t /= div;
	if (t <= min || t >= max)
		return (-1.0);
	return (t);
}
