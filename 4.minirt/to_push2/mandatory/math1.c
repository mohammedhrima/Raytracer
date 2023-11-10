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

unsigned int	rand_pcg(unsigned int *rng_state)
{
	unsigned int		state;
	unsigned int		word;

	state = *rng_state;
	*rng_state = state * 747796405u + 2891336453u;
	word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return ((word >> 22u) ^ word);
}

float	random_float(t_win *win, float min, float max)
{
	return (min + (rand_pcg(&win->rng_state) * (1.0 / UINT_MAX)
		) * (max - min));
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
	return (sqrtf(length_squared(v)));
}
