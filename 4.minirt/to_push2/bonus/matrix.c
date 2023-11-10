/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 20:22:07 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/07 20:22:08 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_vector	matrix_mul_vector(t_matrix m, t_vector v)
{
	t_vector	res;

	res.x = m.e[0][0] * v.x + m.e[0][1] * v.y + m.e[0][2] * v.z;
	res.y = m.e[1][0] * v.x + m.e[1][1] * v.y + m.e[1][2] * v.z;
	res.z = m.e[2][0] * v.x + m.e[2][1] * v.y + m.e[2][2] * v.z;
	return (res);
}

t_matrix	matrix_mul(t_matrix a, t_matrix b)
{
	t_matrix	res;
	int			i;
	int			j;
	int			k;

	i = 0;
	while (i < 3)
	{
		j = 0;
		while (j < 3)
		{
			res.e[i][j] = 0;
			k = 0;
			while (k < 3)
			{
				res.e[i][j] += a.e[i][k] * b.e[k][j];
				k++;
			}
			j++;
		}
		i++;
	}
	return (res);
}

t_matrix	x_rotation(float a)
{
	float	c;
	float	s;

	c = cosf(a);
	s = sinf(a);
	return ((t_matrix){
		{{1, 0, 0},
			{0, c, -s},
			{0, s, c}}
	});
}

t_matrix	z_rotation(float a)
{
	float	c;
	float	s;

	c = cosf(a);
	s = sinf(a);
	return ((t_matrix){
		{{c, -s, 0},
			{s, c, 0},
			{0, 0, 1}}
	});
}

t_matrix	y_rotation(float a)
{
	float	c;
	float	s;

	c = cosf(a);
	s = sinf(a);
	return ((t_matrix){
		{{c, 0, s},
			{0, 1, 0},
			{-s, 0, c}}
	});
}
