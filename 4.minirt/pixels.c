/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pixels.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 12:01:29 by mhrima            #+#    #+#             */
/*   Updated: 2023/10/29 12:03:58 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

t_color	check_pixel(t_color pixel)
{
	pixel = scale(pixel, '/', RAYS_PER_PIXEL);
	if (pixel.x > 1)
		pixel.x = 1;
	if (pixel.y > 1)
		pixel.y = 1;
	if (pixel.z > 1)
		pixel.z = 1;
	return (pixel);
}

t_color	get_color(t_win *win, t_scene *scene, int w, int h)
{
	int		i;
	t_color	pixel;
	t_vec3	pixel_center;
	t_vec3	dir;
	t_ray	ray;

	pixel = (t_color){.x = 0, .y = 0, .z = 0};
	i = 0;
	while (i < RAYS_PER_PIXEL)
	{
		pixel_center = calc(calc(scene->first_pixel, '+', scale(scene->pixel_u,
						'*', w + random_float(0, 1))), '+',
				scale(scene->pixel_v, '*', h + random_float(0, 1)));
		dir = calc(pixel_center, '-', scene->camera);
		ray = (t_ray){.org = scene->camera, .dir = dir};
		pixel = calc(pixel, '+', ray_color(win, ray, 25));
		i++;
	}
	return (check_pixel(pixel));
}

unsigned int	sqrt_color(t_color pixel)
{
	return (((int)(255.999 * sqrtf(pixel.x)) << 16) | ((int)(255.999
				* sqrtf(pixel.y)) << 8) | ((int)(255.999 * sqrtf(pixel.z))));
}

void	set_pixels(t_win *win)
{
	int		h;
	int		w;
	t_color	pixel;
	char	*dst;

	h = 0;
	w = 0;
	while (h < win->height)
	{
		w = 0;
		while (w < win->width)
		{
			pixel = get_color(win, &win->scene, w, h);
			dst = win->addr + (h * win->line_length + w * (win->bits_per_pixel
						/ 8));
			*(unsigned int *)dst = sqrt_color(pixel);
			w++;
		}
		printf("remaining: %d\n", win->height - h);
		h++;
	}
}
