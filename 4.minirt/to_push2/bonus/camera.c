/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:34:53 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:37:13 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	update_camera_position(t_minirt *mrt)
{
	t_vector	camera_dp;

	camera_dp = (t_vector){0};
	if (mrt->is_pressed[CAMERA_FORWARD])
		camera_dp = vector_add(camera_dp, vector_scale(mrt->camera_z, -1));
	if (mrt->is_pressed[CAMERA_BACKWARD])
		camera_dp = vector_add(camera_dp, vector_scale(mrt->camera_z, 1));
	if (mrt->is_pressed[CAMERA_LEFT])
		camera_dp = vector_add(camera_dp, vector_scale(mrt->camera_x, -1));
	if (mrt->is_pressed[CAMERA_RIGHT])
		camera_dp = vector_add(camera_dp, vector_scale(mrt->camera_x, 1));
	if (mrt->is_pressed[CAMERA_UP])
		camera_dp = vector_add(camera_dp, vector_scale(mrt->camera_y, 1));
	if (mrt->is_pressed[CAMERA_DOWN])
		camera_dp = vector_add(camera_dp, vector_scale(mrt->camera_y, -1));
	if (camera_dp.x || camera_dp.y || camera_dp.z)
		mrt->accumulate_frame = 0;
	if (vector_dot(camera_dp, camera_dp))
		camera_dp = vector_normalize(camera_dp);
	mrt->camera_p = vector_add(mrt->camera_p,
			vector_scale(camera_dp, 0.3));
}

void	update_camera(t_minirt *mrt)
{
	t_matrix	camera_rotation_mat;

	camera_rotation_mat = matrix_mul(z_rotation(mrt->camera_rotation.z),
			matrix_mul(y_rotation(mrt->camera_rotation.y),
				x_rotation(mrt->camera_rotation.x)));
	mrt->camera_dir = matrix_mul_vector(camera_rotation_mat,
			mrt->camera_scene_dir);
	mrt->camera_up = matrix_mul_vector(camera_rotation_mat,
			mrt->camera_scene_up);
	mrt->camera_z = vector_normalize(vector_scale(mrt->camera_dir, -1));
	mrt->camera_x = vector_normalize(vector_cross(mrt->camera_up,
				mrt->camera_z));
	mrt->camera_y = vector_normalize(vector_cross(mrt->camera_z,
				mrt->camera_x));
	mrt->pixel_du = vector_scale(mrt->camera_x, mrt->film_width / mrt->width);
	mrt->pixel_dv = vector_scale(mrt->camera_y, mrt->film_height / mrt->height);
	update_camera_position(mrt);
	mrt->film_center = vector_sub(mrt->camera_p,
			vector_scale(mrt->camera_z, mrt->camera_focal_length));
}
