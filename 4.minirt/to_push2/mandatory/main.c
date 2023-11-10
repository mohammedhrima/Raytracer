/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:09:00 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 13:21:40 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	mlx_exit(t_win *win)
{
	close_window(win);
	return (0);
}

int	main(int ac, char **av)
{
	t_win	*win;

	win = new_window(HEIGHT, WIDTH);
	parse_file(win, ac, av);
	win->win = mlx_new_window(win->mlx, win->width, win->height,
			"Mini Raytracer");
	if (!win->win)
		error_exit("mlx_new_window failed");
	init_scene(win);
	mlx_hook(win->win, 2, 1L << 0, listen_on_key, win);
	mlx_hook(win->win, 17, 0, mlx_exit, win);
	mlx_loop_hook(win->mlx, draw, win);
	mlx_loop(win->mlx);
	close_window(win);
}
