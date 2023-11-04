/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vectors1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 12:09:44 by mhrima            #+#    #+#             */
/*   Updated: 2023/10/29 12:09:44 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

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
	if (c == 'x')
	{
		return ((t_vec3){
			.x = l.y * r.z - l.z * r.y,
			.y = l.z * r.x - l.x * r.z,
			.z = l.x * r.y - l.y * r.x});
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

float	unit_dot(t_vec3 u, t_vec3 v)
{
	return (u.x * v.x + u.y * v.y + u.z * v.z);
}
