/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_object1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 23:24:00 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 23:25:33 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

float	hit_sphere(t_ray *ray, t_obj sphere, float max)
{
	t_vec3	oc;
	t_equ	equ;

	oc = calc(ray->org, '-', sphere.center);
	equ.a = length_squared(ray->dir);
	equ.b = dot(oc, ray->dir);
	equ.c = length_squared(oc) - sphere.radius * sphere.radius;
	equ.delta = equ.b * equ.b - equ.a * equ.c;
	if (equ.delta < .0)
		return (-1.0);
	equ.delta = sqrtf(equ.delta);
	equ.sol = (-equ.b - equ.delta) / equ.a;
	if (equ.sol <= ZERO || equ.sol >= max)
		equ.sol = (-equ.b + equ.delta) / equ.a;
	if (equ.sol <= ZERO || equ.sol >= max)
		return (-1.0);
	return (equ.sol);
}

float	hit_plan(t_ray *ray, t_obj plan, float max)
{
	float	t;
	float	div;

	t = -plan.d - dot(plan.normal, ray->org);
	div = dot(ray->dir, plan.normal);
	if (fabsf(div) <= ZERO)
		return (-1.0);
	t /= div;
	if (t <= ZERO || t >= max)
		return (-1.0);
	return (t);
}
