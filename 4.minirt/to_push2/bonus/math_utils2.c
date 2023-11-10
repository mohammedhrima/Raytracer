/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:50:36 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:50:46 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_vector	get_up_vector(t_vector forward)
{
	t_vector	up;
	float		d;

	up = (t_vector){0, 1, 0};
	d = vector_dot(up, forward);
	if (d > 0.9)
		up = (t_vector){0, 0, 1};
	else if (d < -0.9)
		up = (t_vector){0, 0, -1};
	return (up);
}

t_vector	ray_at(t_vector ray_origin, t_vector ray_dir, float t)
{
	return (vector_add(ray_origin, vector_scale(ray_dir, t)));
}

float	float_sign(float x)
{
	if (x < 0)
		return (-1);
	else if (x == 0)
		return (0);
	else
		return (1);
}
