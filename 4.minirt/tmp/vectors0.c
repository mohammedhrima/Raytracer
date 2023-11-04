/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vectors0.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 12:07:21 by mhrima            #+#    #+#             */
/*   Updated: 2023/10/29 12:09:36 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

float	length_squared(t_vec3 v)
{
	return (pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

float	length(t_vec3 v)
{
	return (sqrt(length_squared(v)));
}

t_vec3	unit_vector(t_vec3 v)
{
	float	f;

	f = length(v);
	if (f <= ZERO)
		return ((t_vec3){});
	return (scale(v, '/', f));
}

t_vec3	random_unit_vector(void)
{
	t_vec3	u;
	t_vec3	v;

	while (1)
	{
		u = (t_vec3){.x = random_float(-1, 1), .y = random_float(-1, 1),
			.z = random_float(-1, 1)};
		if (length_squared(u) <= 1)
			break ;
	}
	v = unit_vector(u);
	return (v);
}

t_vec3	point_at(t_ray ray, float t)
{
	return ((t_vec3){.x = ray.org.x + t * ray.dir.x, .y = ray.org.y + t
		* ray.dir.y, .z = ray.org.z + t * ray.dir.z});
}
