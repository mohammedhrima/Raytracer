/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 20:31:24 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:50:48 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

float	random_float(float min, float max)
{
	return (min + ((double)rand() / RAND_MAX) * (max - min));
}

float	square(float x)
{
	return (x * x);
}

float	lerp(float a, float t, float b)
{
	return ((1 - t) * a + b * t);
}

int	imin(int x, int y)
{
	if (x < y)
		return (x);
	return (y);
}

int	imax(int x, int y)
{
	if (x > y)
		return (x);
	return (y);
}
