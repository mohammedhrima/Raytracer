/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:31:39 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 13:22:59 by zfarini          ###   ########.fr       */
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
	set_pixels(win);
	mlx_put_image_to_window(win->mlx, win->win, win->img, 0, 0);
	win->scene.frame++;
	return (0);
}

void	add_objects(t_win *win, t_obj obj)
{
	t_obj	*tmp;

	if (!win->scene.objects)
	{
		win->scene.obj_len = 5;
		win->scene.objects = memory(win->scene.obj_len * sizeof(t_obj));
	}
	if (win->scene.pos == win->scene.obj_len)
	{
		tmp = memory(win->scene.obj_len * 2 * sizeof(t_obj));
		ft_memcpy(tmp, win->scene.objects, win->scene.obj_len * sizeof(t_obj));
		free(win->scene.objects);
		win->scene.objects = tmp;
		win->scene.obj_len *= 2;
	}
	win->scene.objects[win->scene.pos++] = obj;
}

t_win	*new_window(int height, int width)
{
	t_win	*win;

	win = memory(sizeof(t_win));
	win->width = width;
	win->height = height;
	win->mlx = mlx_init();
	if (!win->mlx)
		error_exit("mlx_init failed");
	win->img = mlx_new_image(win->mlx, win->width, win->height);
	if (!win->img)
		error_exit("mlx_new_image failed");
	win->addr = mlx_get_data_addr(win->img, &win->bits_per_pixel,
			&win->line_length, &win->endian);
	if (!win->addr)
		error_exit("mlx_get_data_addr failed");
	win->scene.sum = memory(sizeof(t_vec3) * height * width);
	return (win);
}
