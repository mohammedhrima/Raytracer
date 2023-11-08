/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing0.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:39:31 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:48:43 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	skip(char *str, char c, int i)
{
	if (str[i] == '\0')
	{
		printf("Error unexpected EOL\n");
		exit(1);
	}
	if (str[i] != c)
	{
		printf("Error 0 unexpected '%c'\n", str[i]);
		exit(1);
	}
	i++;
	if (str[i] == '\0')
	{
		printf("Error unexpected EOL\n");
		exit(1);
	}
	return (i);
}

float	get_floating_part(char *str, int *ptr, int sign)
{
	float	res;
	int		i;
	float	j;

	i = *ptr;
	res = 0.0;
	if (str[i] == '.')
	{
		i++;
		j = 0.1;
		while (ft_isdigit(str[i]))
		{
			res = res + sign * j * (str[i] - '0');
			j /= 10;
			i++;
		}
	}
	*ptr = i;
	return (res);
}

float	get_number(char *str, int *ptr, int sign)
{
	float	res;
	int		i;

	i = *ptr;
	res = 0.0;
	if (!ft_isdigit(str[i]))
	{
		printf("Error 1 unexpected '%c'\n", str[i]);
		exit(1);
	}
	while (ft_isdigit(str[i]))
	{
		res = 10 * res + sign * (str[i] - '0');
		i++;
	}
	res += get_floating_part(str, &i, sign);
	*ptr = i;
	return (res);
}

float	parse_number(char *str, int *ptr)
{
	int		i;
	float	res;
	int		sign;

	i = *ptr;
	res = 0.0;
	sign = 1;
	while (str[i] && ft_isspace(str[i]))
		i++;
	if (ft_strchr("+-", str[i]))
	{
		if (str[i] == '-')
			sign = -1;
		i = skip(str, str[i], i);
	}
	res = get_number(str, &i, sign);
	while (str[i] && ft_isspace(str[i]))
		i++;
	*ptr = i;
	return (res);
}

void	parse_error(float num, float min, float max, char *msg)
{
	if (num < min || num > max)
	{
		printf("Error: %s should be in range [%f,%f]\n", msg, min, max);
		exit(1);
	}
}
