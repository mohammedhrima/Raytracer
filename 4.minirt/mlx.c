/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 10:20:37 by mhrima            #+#    #+#             */
/*   Updated: 2023/10/29 12:01:16 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	close_window(t_win *win)
{
	mlx_destroy_window(win->mlx, win->win);
	exit(0);
}

int	listen_on_key(int code, t_win *win)
{
	if (code == ESC)
		close_window(win);
	return (0);
}

int	draw(void *ptr)
{
	t_win	*win;

	win = (t_win *)ptr;
	mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
	return (0);
}

t_win	*new_window(int height, int width)
{
	t_win	*win;

	win = ft_calloc(1, sizeof(t_win));
	win->width = width;
	win->height = height;
	win->mlx = mlx_init();
	win->win = mlx_new_window(win->mlx, win->width, win->height,
			"Mini Raytracer");
	win->img = mlx_new_image(win->mlx, win->width, win->height);
	win->addr = mlx_get_data_addr(win->img, &win->bits_per_pixel,
			&win->line_length, &win->endian);
	return (win);
}
