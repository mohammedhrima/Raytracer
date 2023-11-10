/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 20:36:24 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/09 11:56:28 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

typedef struct s_bilinear_sample
{
	float	sx;
	float	sy;
	float	fx;
	float	fy;
	int		x;
	int		y;
	int		x_plus;
	int		y_plus;
	float	h[4];
}	t_bilinear_sample;

float	sample_bump_map(t_object *e, t_texture *b, float u, float v)
{
	t_bilinear_sample	s;

	u *= e->bump_scale_u;
	v *= e->bump_scale_v;
	u -= floorf(u);
	v -= floorf(v);
	s.sx = u * b->width - 0.5f;
	s.sy = v * b->height - 0.5f;
	s.x = floorf(s.sx);
	s.y = floorf(s.sy);
	s.fx = s.sx - floorf(s.sx);
	s.fy = s.sy - floorf(s.sy);
	s.x_plus = imin(s.x + 1, b->width - 1);
	s.y_plus = imin(s.y + 1, b->height - 1);
	s.x = imin(s.x, b->width - 1);
	s.y = imin(s.y, b->height - 1);
	s.x = imax(s.x, 0);
	s.y = imax(s.y, 0);
	s.h[0] = ((*(b->pixels + s.y * b->width + s.x)) & 0xFF) / 255.0f;
	s.h[1] = ((*(b->pixels + s.y * b->width + s.x_plus)) & 0xFF) / 255.0f;
	s.h[2] = ((*(b->pixels + s.y_plus * b->width + s.x)) & 0xFF) / 255.0f;
	s.h[3] = ((*(b->pixels + s.y_plus * b->width + s.x_plus)) & 0xFF) / 255.0f;
	return (lerp(lerp(s.h[0], s.fx, s.h[1]), s.fy, lerp(s.h[2], s.fx, s.h[3])));
}

t_vector	calc_pixel_position(t_minirt *mrt, int x, int y)
{
	float		film_x;
	float		film_y;
	t_vector	pixel;

	film_y = -1 + ((float)y / mrt->height) * 2;
	film_x = -1 + (((float)x / mrt->width)) * 2;
	pixel = mrt->film_center;
	pixel = vector_add(pixel, vector_scale(mrt->camera_x,
				film_x * (mrt->film_width * 0.5f)));
	pixel = vector_add(pixel, vector_scale(mrt->camera_y,
				film_y * (mrt->film_height * 0.5f)));
	pixel = vector_add(pixel,
			vector_add(vector_scale(mrt->pixel_du, random_float(0, 1)),
				vector_scale(mrt->pixel_dv, random_float(0, 1))));
	return (pixel);
}

void	raytrace_pixel(t_minirt *mrt, int x, int y, int pixel_index)
{
	t_vector	pixel;
	t_vector	color;

	pixel = calc_pixel_position(mrt, x, y);
	color = raytrace(mrt, mrt->camera_p,
			vector_normalize(vector_sub(pixel, mrt->camera_p)));
	if (!mrt->accumulate_frame)
		*(mrt->color_sum + pixel_index) = (t_vector){0};
	color = vector_add(*(mrt->color_sum + pixel_index), color);
	*(mrt->color_sum + pixel_index) = color;
	color = vector_scale(color, 1.f / (mrt->accumulate_frame + 1));
	if (color.x > 1)
		color.x = 1;
	if (color.y > 1)
		color.y = 1;
	if (color.z > 1)
		color.z = 1;
	*(mrt->pixels + pixel_index)
		= ((uint32_t)(sqrtf(color.x) * 255 + 0.5f) << 16)
		| ((uint32_t)(sqrtf(color.y) * 255 + 0.5f) << 8)
		| ((uint32_t)(sqrtf(color.z) * 255 + 0.5f) << 0);
}

int	minirt(t_minirt *mrt)
{
	int				y;
	int				x;

	update_camera(mrt);
	y = 0;
	while (y < mrt->height)
	{
		x = 0;
		while (x < mrt->width)
		{
			raytrace_pixel(mrt, x, y, (mrt->height - y - 1) * mrt->width + x);
			x++;
		}
		y++;
	}
	mlx_put_image_to_window(mrt->mlx, mrt->window, mrt->image, 0, 0);
	mrt->frame++;
	mrt->accumulate_frame++;
	return (0);
}
