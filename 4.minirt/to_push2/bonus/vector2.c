/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:48:50 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:49:31 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_vector	vector_normalize(t_vector v)
{
	float	len;

	len = vector_length(v);
	if (len < EPSILON)
		return ((t_vector){0});
	len = 1.f / len;
	return ((t_vector){v.x * len, v.y * len, v.z * len});
}

float	vector_dot(t_vector a, t_vector b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

t_vector	vector_cross(t_vector a, t_vector b)
{
	return ((t_vector){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	});
}

t_vector	vector_reflect(t_vector a, t_vector n)
{
	return (vector_sub(a, vector_scale(n, 2 * vector_dot(a, n))));
}
