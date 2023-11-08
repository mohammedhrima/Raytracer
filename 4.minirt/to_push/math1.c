/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:00:09 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 22:02:48 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

unsigned int	rand_pcg(void)
{
	static unsigned int	rng_state;
	unsigned int		state;
	unsigned int		word;

	state = rng_state;
	rng_state = rng_state * 747796405u + 2891336453u;
	word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return ((word >> 22u) ^ word);
}

float	random_float(float min, float max)
{
	static double	one_over_uint_max;

	one_over_uint_max = 1.0 / UINT_MAX;
	return (min + (rand_pcg() * one_over_uint_max) * (max - min));
}

float	degrees_to_radians(float degrees)
{
	return (degrees * PI / 180.0);
}

float	length_squared(t_vec3 v)
{
	return (pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

float	length(t_vec3 v)
{
	return (sqrt(length_squared(v)));
}
