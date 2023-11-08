/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:31:39 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:10:06 by mhrima           ###   ########.fr       */
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
	printf("%d\n", code);
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

void	add_objects(t_win *win, t_obj obj)
{
	t_obj	*tmp;

	if (!win->scene.objects)
	{
		win->scene.obj_len = 100;
		win->scene.objects = memory(win->scene.obj_len * sizeof(t_obj));
	}
	win->scene.objects[win->scene.pos++] = obj;
	if (win->scene.pos + 4 == win->scene.obj_len)
	{
		tmp = memory(win->scene.obj_len * 2 * sizeof(t_obj));
		ft_memcpy(tmp, win->scene.objects, win->scene.obj_len * sizeof(t_obj));
		win->scene.objects = tmp;
		win->scene.obj_len *= 2;
	}
}

t_win	*new_window(int height, int width)
{
	t_win	*win;

	win = memory(sizeof(t_win));
	win->width = width;
	win->height = height;
	win->mlx = mlx_init();
	win->img = mlx_new_image(win->mlx, win->width, win->height);
	win->addr = mlx_get_data_addr(win->img, &win->bits_per_pixel,
			&win->line_length, &win->endian);
	return (win);
}
