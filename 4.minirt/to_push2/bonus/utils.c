/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 20:29:06 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/09 12:22:48 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	free_objects(t_minirt *mrt)
{
	int	i;

	i = 0;
	while (i < mrt->object_count)
	{
		if (mrt->objects[i].texture)
		{
			if (mrt->objects[i].texture->image)
				mlx_destroy_image(mrt->mlx, mrt->objects[i].texture->image);
			else
				free(mrt->objects[i].texture->pixels);
		}
		free(mrt->objects[i].texture);
		if (mrt->objects[i].bump)
		{
			if (mrt->objects[i].bump->image)
				mlx_destroy_image(mrt->mlx, mrt->objects[i].bump->image);
			else
				free(mrt->objects[i].bump->pixels);
		}
		free(mrt->objects[i].bump);
		i++;
	}
}

void	mrt_exit(t_minirt *mrt, char *msg)
{
	if (msg)
		printf("Error\n%s\n", msg);
	if (mrt->mlx)
	{
		mlx_destroy_image(mrt->mlx, mrt->image);
		mlx_destroy_window(mrt->mlx, mrt->window);
	}
	free_objects(mrt);
	free(mrt->texture_name);
	free(mrt->curr_line);
	free(mrt->lights);
	free(mrt->objects);
	free(mrt->color_sum);
	exit(0);
}

void	ft_memset(void *dest, int value, size_t size)
{
	size_t	i;

	i = 0;
	while (i < size)
	{
		*((char *)dest + i) = value;
		i++;
	}
}

void	ft_memcpy(void *dest, void *src, size_t size)
{
	size_t	i;

	i = 0;
	while (i < size)
	{
		*((char *)dest + i) = *((char *)src + i);
		i++;
	}
}

int	ft_isspace(char c)
{
	return (c == '\t' || c == ' ');
}
