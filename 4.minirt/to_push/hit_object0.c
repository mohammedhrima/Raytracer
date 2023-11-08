/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hit_object0.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 23:17:06 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 23:25:28 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	check_solution(t_equ *equ, t_ray *ray, t_obj cylin, float max)
{
	float	x1;
	float	x2;
	t_vec3	p;
	float	cal;

	equ->delta = sqrt(equ->delta);
	x1 = (-equ->b + equ->delta) / (2 * equ->a);
	x2 = (-equ->b - equ->delta) / (2 * equ->a);
	if (x1 < ZERO || x1 > max)
		x1 = -1.0;
	if (x2 < ZERO || x2 > max)
		x2 = -1.0;
	if (x1 == -1 || x2 < x1)
		equ->sol = x2;
	if (x2 == -1 || x1 < x2)
		equ->sol = x1;
	cal = fabsf(dot(cylin.normal, calc(point_at(ray, equ->sol), '-',
					cylin.center)));
	if (cal > cylin.height / 2)
		equ->sol = -1.0;
	p = point_at(ray, equ->sol);
	*(equ->hit_normal) = calc(calc(p, '-', scale(cylin.normal, '*', dot(calc(p,
							'-', cylin.center), cylin.normal))), '+',
			cylin.center);
}

void	check_first_side(t_equ *equ, t_ray *ray, t_obj cylin, float max)
{
	t_vec3	c;
	float	d;
	t_vec3	p;

	c = calc(cylin.center, '+', scale(cylin.normal, '*', cylin.height / 2));
	d = hit_plan(ray, (t_obj){.normal = cylin.normal, .d = -dot(c,
				cylin.normal)}, max);
	if (d > 0)
	{
		p = point_at(ray, d);
		if (length(calc(p, '-', c)) <= cylin.radius && (equ->sol < 0
				|| d < equ->sol))
		{
			equ->sol = d;
			*(equ->hit_normal) = cylin.normal;
		}
	}
}

void	check_second_side(t_equ *equ, t_ray *ray, t_obj cylin, float max)
{
	t_vec3	c;
	float	d;
	t_vec3	p;

	c = calc(cylin.center, '-', scale(cylin.normal, '*', cylin.height / 2));
	d = hit_plan(ray,
			(t_obj){
			.normal = scale(cylin.normal, '*', -1),
			.d = -dot(c, scale(cylin.normal, '*', -1))},
			max);
	if (d > 0)
	{
		p = point_at(ray, d);
		if (length(calc(p, '-', c)) <= cylin.radius && (equ->sol < 0
				|| d < equ->sol))
		{
			equ->sol = d;
			*(equ->hit_normal) = scale(cylin.normal, '*', -1);
		}
	}
}

float	hit_cylinder(t_ray *ray, t_obj cylin, float max, t_vec3 *hit_normal)
{
	t_equ	equ;
	t_vec3	ran;
	t_vec3	u;
	t_vec3	v;

	ran = (t_vec3){0, 0, 1};
	if (fabsf(dot(ran, cylin.normal)) >= 0.9f)
		ran = (t_vec3){1, 0, 0};
	u = unit_vector(cross(ran, cylin.normal));
	v = unit_vector(cross(cylin.normal, u));
	equ.hit_normal = hit_normal;
	equ.a = pow2(dot(u, ray->dir)) + pow2(dot(v, ray->dir));
	equ.b = 2 * (dot(calc(ray->org, '-', cylin.center), u) * dot(u, ray->dir)
			+ dot(calc(ray->org, '-', cylin.center), v) * dot(v, ray->dir));
	equ.c = pow2(dot(calc(ray->org, '-', cylin.center), u))
		+ pow2(dot(calc(ray->org, '-', cylin.center), v)) - pow2(cylin.radius);
	equ.delta = equ.b * equ.b - 4 * equ.a * equ.c;
	equ.sol = -1.0;
	if (equ.delta >= 0)
		check_solution(&equ, ray, cylin, max);
	check_first_side(&equ, ray, cylin, max);
	check_second_side(&equ, ray, cylin, max);
	if (equ.sol < ZERO || equ.sol > max)
		return (-1);
	return (equ.sol);
}
