/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 10:20:41 by mhrima            #+#    #+#             */
/*   Updated: 2023/10/29 12:06:47 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	*ft_memset(void *pointer, int c, size_t len)
{
	size_t			i;
	unsigned char	*destionation;

	destionation = (unsigned char *)pointer;
	i = 0;
	while (i < len)
	{
		*destionation = (unsigned char)c;
		destionation++;
		i++;
	}
	return (pointer);
}

void	*ft_calloc(size_t count, size_t size)
{
	void	*array;

	if (count && size > SIZE_MAX / count)
		return (NULL);
	array = (void *)malloc(count * size);
	if (!array)
	{
		printf("Error allocation in calloc\n");
		exit(-1);
	}
	ft_memset(array, 0, count * size);
	return (array);
}

unsigned	rand_pcg(void)
{
	static unsigned	rng_state;
	unsigned		state;
	unsigned		word;

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
