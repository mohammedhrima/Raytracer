/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:41:34 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:46:13 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

char	*get_next_line(t_minirt *mrt, int fd)
{
	char	c;
	int		len;

	len = 0;
	mrt->curr_line = 0;
	while (1)
	{
		mrt->bytes_read = read(fd, &c, 1);
		if (mrt->bytes_read == 0)
			break ;
		else if (mrt->bytes_read < 0)
			mrt_exit(mrt, "Couldn't read the scene file");
		else if (c == '\n')
		{
			if (!mrt->curr_line)
				mrt->curr_line = ft_calloc(mrt, 1, 1);
			break ;
		}
		mrt->new_line = ft_calloc(mrt, len + 2, 1);
		ft_memcpy(mrt->new_line, mrt->curr_line, len);
		mrt->new_line[len++] = c;
		free(mrt->curr_line);
		mrt->curr_line = mrt->new_line;
	}
	return (mrt->curr_line);
}

void	skip_spaces(char **line)
{
	while (ft_isspace(**line))
		*line = *line + 1;
}

int	parse_int_value(t_minirt *mrt, char **line, int sign)
{
	int	res;
	int	l;

	res = 0;
	l = 0;
	while (**line >= '0' && **line <= '9')
	{
		if (sign > 0)
		{
			if (res > (INT_MAX - (**line - '0')) / 10)
				mrt_exit(mrt, "Integer overflow");
			res = res * 10 + (**line - '0');
		}
		else
		{
			if (res < (INT_MIN + (**line - '0')) / 10)
				mrt_exit(mrt, "Integer overflow");
			res = res * 10 - (**line - '0');
		}
		*line = *line + 1;
		l = 1;
	}
	if (!l)
		mrt_exit(mrt, "Parse error");
	return (res);
}

int	parse_int(t_minirt *mrt, char **line, int skip)
{
	int	sign;

	sign = 1;
	if (skip)
		skip_spaces(line);
	if (skip && (**line == '+' || **line == '-'))
	{
		if (**line == '-')
			sign = -1;
		*line = *line + 1;
	}
	return (parse_int_value(mrt, line, sign));
}
