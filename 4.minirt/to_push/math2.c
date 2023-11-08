/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:03:17 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 22:06:41 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

float	pow2(float x)
{
	return (x * x);
}

t_vec3	point_at(t_ray *ray, float t)
{
	return ((t_vec3){.x = ray->org.x + t * ray->dir.x, .y = ray->org.y + t
		* ray->dir.y, .z = ray->org.z + t * ray->dir.z});
}

t_vec3	calc(t_vec3 l, char c, t_vec3 r)
{
	if (c == '+')
		return ((t_vec3){.x = l.x + r.x, .y = l.y + r.y, .z = l.z + r.z});
	if (c == '-')
		return ((t_vec3){.x = l.x - r.x, .y = l.y - r.y, .z = l.z - r.z});
	if (c == '*')
		return ((t_vec3){.x = l.x * r.x, .y = l.y * r.y, .z = l.z * r.z});
	if (c == '/')
	{
		if (r.x == 0.0 || r.y == 0.0 || r.z == 0.0)
		{
			printf("Error 2: dividing by 0\n");
			exit(1);
		}
		return ((t_vec3){.x = l.x / r.x, .y = l.y / r.y, .z = l.z / r.z});
	}
	return ((t_vec3){});
}

t_vec3	scale(t_vec3 v, char c, float t)
{
	if (c == '*')
		return ((t_vec3){.x = t * v.x, .y = t * v.y, .z = t * v.z});
	if (c == '/')
	{
		if (t == 0)
		{
			printf("Error 1: dividing by 0\n");
			exit(1);
		}
		return ((t_vec3){.x = v.x / t, .y = v.y / t, .z = v.z / t});
	}
	return ((t_vec3){});
}

t_vec3	cross(t_vec3 u, t_vec3 v)
{
	return ((t_vec3){u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x});
}
