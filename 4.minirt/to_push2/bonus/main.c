/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:36:26 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/09 12:26:49 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	mlx_exit(t_minirt *mrt)
{
	mrt_exit(mrt, 0);
	return (0);
}

void	minirt_init(t_minirt *mrt, int width, int height, char *scene_filename)
{
	mrt->width = width;
	mrt->height = height;
	load_scene(mrt, scene_filename);
	mrt->mlx = mlx_init();
	if (!mrt->mlx)
		mrt_exit(mrt, "mlx_init failed");
	mrt->window = mlx_new_window(mrt->mlx, mrt->width, mrt->height, "minirt");
	if (!mrt->window)
		mrt_exit(mrt, "mlx_new_window failed");
	mrt->image = mlx_new_image(mrt->mlx, mrt->width, mrt->height);
	if (!mrt->image)
		mrt_exit(mrt, "mlx_new_image failed");
	mrt->pixels = (uint32_t *)mlx_get_data_addr(mrt->image,
			&mrt->bits_per_pixel, &mrt->line_length, &mrt->endian);
	if (!mrt->pixels || mrt->bits_per_pixel != 32 || mrt->endian)
		mrt_exit(mrt, "mlx_get_data_addr failed");
	mrt->camera_focal_length = 1;
	if (mrt->camera_fov > 179)
		mrt->camera_fov = 179;
	mrt->film_width = tanf(mrt->camera_fov * 0.5f * (PI / 180))
		* mrt->camera_focal_length * 2;
	mrt->film_height = mrt->film_width;
	mrt->color_sum = ft_calloc(mrt, mrt->width * mrt->height,
			sizeof(*mrt->color_sum));
}

void	run_mlx(t_minirt *mrt)
{
	uint32_t	mouse_mask;

	mouse_mask = (1 << 15) | (1 << 0) | (1 << 2)
		| (1 << 19) | (1 << 3) | (1 << 1) | (1 << 4)
		| (1 << 5) | (1 << 6) | (1 << 8) | (1 << 16) | (1 << 23);
	mlx_hook(mrt->window, 2, (1 << 0) | (1 << 2), on_key_down, mrt);
	mlx_hook(mrt->window, 3, (1 << 1) | (1 << 3), on_key_up, mrt);
	mlx_hook(mrt->window, 4, mouse_mask, on_mouse_down, mrt);
	mlx_hook(mrt->window, 5, mouse_mask, on_mouse_up, mrt);
	mlx_hook(mrt->window, 6, mouse_mask, on_mouse_movement, mrt);
	mlx_hook(mrt->window, 17, 0, mlx_exit, mrt);
	mlx_loop_hook(mrt->mlx, minirt, mrt);
	mlx_loop(mrt->mlx);
}

int	main(int argc, char **argv)
{
	t_minirt	mrt;
	char		*scene_filename;

	scene_filename = argv[1];
	if (argc != 2
		|| ft_strlen(scene_filename) < 3 || ft_strcmp(scene_filename
			+ ft_strlen(scene_filename) - 3, ".rt"))
	{
		printf("ERROR\nexpected a \".rt\" as argument\n");
		return (1);
	}
	ft_memset(&mrt, 0, sizeof(mrt));
	minirt_init(&mrt, WINDOW_WIDTH, WINDOW_HEIGHT, scene_filename);
	run_mlx(&mrt);
}
