/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 20:33:37 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 13:09:08 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	on_mouse_down(int button, int x, int y, t_minirt *mrt)
{
	(void)button;
	mrt->is_mouse_down = 1;
	mrt->prev_mouse_x = x;
	mrt->prev_mouse_y = y;
	return (0);
}

int	on_mouse_up(int button, int x, int y, t_minirt *mrt)
{
	(void)button;
	(void)x;
	(void)y;
	mrt->is_mouse_down = 0;
	return (0);
}

int	on_mouse_movement(int x, int y, t_minirt *mrt)
{
	float	dx;
	float	dy;

	if (!mrt->is_mouse_down)
		return (0);
	dx = (x - mrt->prev_mouse_x);
	dy = (y - mrt->prev_mouse_y);
	if (dx || dy)
		mrt->accumulate_frame = 0;
	mrt->camera_rotation.x += dy / (mrt->height * 0.5f) * PI * 0.5;
	mrt->camera_rotation.y += dx / (mrt->width * 0.5f) * PI * 0.5;
	mrt->prev_mouse_x = x;
	mrt->prev_mouse_y = y;
	return (0);
}

int	on_key_down(int keycode, t_minirt *mrt)
{
	if (keycode == KEYCODE_ESCAPE)
		mrt_exit(mrt, 0);
	if (keycode == KEYCODE_W)
		mrt->is_pressed[CAMERA_FORWARD] = 1;
	else if (keycode == KEYCODE_S)
		mrt->is_pressed[CAMERA_BACKWARD] = 1;
	else if (keycode == KEYCODE_A)
		mrt->is_pressed[CAMERA_LEFT] = 1;
	else if (keycode == KEYCODE_D)
		mrt->is_pressed[CAMERA_RIGHT] = 1;
	else if (keycode == KEYCODE_Q)
		mrt->is_pressed[CAMERA_UP] = 1;
	else if (keycode == KEYCODE_E)
		mrt->is_pressed[CAMERA_DOWN] = 1;
	return (0);
}

int	on_key_up(int keycode, t_minirt *mrt)
{
	if (keycode == KEYCODE_W)
		mrt->is_pressed[CAMERA_FORWARD] = 0;
	else if (keycode == KEYCODE_S)
		mrt->is_pressed[CAMERA_BACKWARD] = 0;
	else if (keycode == KEYCODE_A)
		mrt->is_pressed[CAMERA_LEFT] = 0;
	else if (keycode == KEYCODE_D)
		mrt->is_pressed[CAMERA_RIGHT] = 0;
	else if (keycode == KEYCODE_Q)
		mrt->is_pressed[CAMERA_UP] = 0;
	else if (keycode == KEYCODE_E)
		mrt->is_pressed[CAMERA_DOWN] = 0;
	return (0);
}
