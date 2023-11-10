/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:43:20 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:43:43 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

float	parse_float_real(char **line)
{
	float	res;
	float	p;

	p = 0.1;
	res = 0;
	if (**line == '.')
	{
		*line = *line + 1;
		while (**line >= '0' && **line <= '9')
		{
			res += p * (**line - '0');
			p /= 10;
			*line = *line + 1;
		}
	}
	return (res);
}

float	parse_float(t_minirt *mrt, char **line)
{
	int		sign;
	float	res;

	skip_spaces(line);
	sign = 1;
	if (**line == '-')
		sign = -1;
	res = parse_int(mrt, line, 1);
	res = (res + parse_float_real(line) * sign);
	return (res);
}

void	skip_comma(t_minirt *mrt, char **line)
{
	skip_spaces(line);
	if (**line != ',')
		mrt_exit(mrt, "Parse error");
	*line = *line + 1;
	skip_spaces(line);
}

t_vector	parse_color(t_minirt *mrt, char **line)
{
	t_vector	c;

	c.x = parse_int(mrt, line, 1);
	skip_comma(mrt, line);
	c.y = parse_int(mrt, line, 1);
	skip_comma(mrt, line);
	c.z = parse_int(mrt, line, 1);
	if (c.x < 0 || c.x > 255 || c.y < 0 || c.y > 255 || c.z < 0 || c.z > 255)
		mrt_exit(mrt, "Expected color value in range [0-255]");
	return (vector_scale(c, 1.f / 255));
}

t_vector	parse_vector(t_minirt *mrt, char **line, int normal)
{
	t_vector	c;

	c.x = parse_float(mrt, line);
	skip_comma(mrt, line);
	c.y = parse_float(mrt, line);
	skip_comma(mrt, line);
	c.z = parse_float(mrt, line);
	if (normal)
	{
		if ((!c.x && !c.y && !c.z) || (c.x < -1 || c.x > 1)
			|| (c.y < -1 || c.y > 1) || (c.z < -1 || c.z > 1))
			mrt_exit(mrt, "Expected normal in range [-1,1]"
				" and a non-zero length");
		c = vector_normalize(c);
	}
	return (c);
}
