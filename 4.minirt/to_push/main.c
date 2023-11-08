/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:09:00 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 22:09:00 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	main(int ac, char **av)
{
	t_win	*win;

	win = new_window(HEIGHT, WIDTH);
	parse_file(win, ac, av);
	init_scene(win);
	set_pixels(win);
	win->win = mlx_new_window(win->mlx, win->width, win->height,
			"Mini Raytracer");
	mlx_hook(win->win, 2, 1L << 0, listen_on_key, win);
	mlx_loop_hook(win->mlx, draw, win);
	mlx_loop(win->mlx);
	close_window(win);
}
