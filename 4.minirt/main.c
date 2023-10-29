/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 10:04:48 by mhrima            #+#    #+#             */
/*   Updated: 2023/10/29 11:57:25 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	init_scene(t_win *win)
{
	win->scene.camera = (t_vec3){.x = 0, .y = 0, .z = FOCAL_LEN};
	win->scene.cam_dir = (t_vec3){.x = 0, .y = 0, .z = -FOCAL_LEN};
	win->scene.w = scale(unit_vector(win->scene.cam_dir), '*', -1);
	win->scene.up = (t_vec3){.x = 0, .y = 1, .z = 0};
	win->scene.screen_height = 2 * tan(degrees_to_radians(20) / 2) * FOCAL_LEN;
	win->scene.screen_width = win->scene.screen_height * ((float)win->width
			/ win->height);
	win->scene.u = unit_vector(calc(win->scene.up, 'x', win->scene.w));
	win->scene.v = unit_vector(calc(win->scene.w, 'x', win->scene.u));
	win->scene.screen_u = scale(win->scene.u, '*', win->scene.screen_width);
	win->scene.screen_v = scale(win->scene.v, '*', -win->scene.screen_height);
	win->scene.pixel_u = scale(win->scene.screen_u, '/', win->width);
	win->scene.pixel_v = scale(win->scene.screen_v, '/', win->height);
	win->scene.center = calc(win->scene.camera, '+', scale(win->scene.w, '*',
				-FOCAL_LEN));
	win->scene.upper_left = calc(win->scene.center, '-',
			scale(calc(win->scene.screen_u, '+', win->scene.screen_v), '/', 2));
	win->scene.first_pixel = calc(win->scene.upper_left, '+',
			scale(calc(win->scene.pixel_u, '+', win->scene.pixel_v), '/', 2));
}

int	main(void)
{
	t_win	*win;

	win = new_window(HEIGHT, WIDTH);
	init_scene(win);
	add_objects(win);
	set_pixels(win);
	mlx_hook(win->win, 2, 1L << 0, listen_on_key, win);
	mlx_loop_hook(win->mlx, draw, win);
	mlx_loop(win->mlx);
}
