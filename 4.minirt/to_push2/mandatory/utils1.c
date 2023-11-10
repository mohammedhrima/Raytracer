/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 00:26:57 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:42:03 by mhrima           ###   ########.fr       */
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

void	*memory(size_t size)
{
	void	*array;

	array = (void *)malloc(size);
	if (!array)
		error_exit("malloc failed");
	ft_memset(array, 0, size);
	return (array);
}

void	error_exit(char *msg)
{
	if (!msg)
		msg = "Parse error";
	printf("Error\n%s\n", msg);
	exit(1);
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
