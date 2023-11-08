/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:48:08 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:48:24 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

t_vec3	parse_color(char *str, int *i)
{
	t_vec3	color;

	color.x = parse_number(str, i);
	*i = skip(str, ',', *i);
	color.y = parse_number(str, i);
	*i = skip(str, ',', *i);
	color.z = parse_number(str, i);
	parse_error(color.x, 0, 255, "RGB");
	parse_error(color.y, 0, 255, "RGB");
	parse_error(color.z, 0, 255, "RGB");
	color.x /= 255.999;
	color.y /= 255.999;
	color.z /= 255.999;
	return (color);
}

t_vec3	parse_normal(char *str, int *i)
{
	t_vec3	normal;

	normal.x = parse_number(str, i);
	*i = skip(str, ',', *i);
	normal.y = parse_number(str, i);
	*i = skip(str, ',', *i);
	normal.z = parse_number(str, i);
	parse_error(normal.x, -1, 1, "normal");
	parse_error(normal.y, -1, 1, "normal");
	parse_error(normal.z, -1, 1, "normal");
	return (unit_vector(normal));
}

void	parse_ambient_light(t_win *win, char *elems_str, char *str, int i)
{
	float	ambient_light_ratio;

	if (elems_str[(int)str[i]])
	{
		printf("Error: ambient light already exists\n");
		exit(1);
	}
	elems_str[(int)str[i++]] = 1;
	ambient_light_ratio = parse_number(str, &i);
	parse_error(ambient_light_ratio, 0, 1, "ambient light ratio");
	win->scene.ambient_light_color = parse_color(str, &i);
	if (str[i])
	{
		printf("Error (A): unexpected '%c'\n", str[i]);
		exit(1);
	}
}

void	parse_camera(t_win *win, char *elems_str, char *str, int i)
{
	if (elems_str[(int)str[i]])
	{
		printf("Error: camera already exists\n");
		exit(1);
	}
	elems_str[(int)str[i++]] = 1;
	win->scene.camera.x = parse_number(str, &i);
	i = skip(str, ',', i);
	win->scene.camera.y = parse_number(str, &i);
	i = skip(str, ',', i);
	win->scene.camera.z = parse_number(str, &i);
	win->scene.cam_dir = parse_normal(str, &i);
	win->scene.fov = parse_number(str, &i);
	parse_error(win->scene.fov, 0, 180, "fov");
	win->scene.fov = degrees_to_radians(win->scene.fov) / 2;
	win->scene.focal_len = 1;
	if (str[i])
	{
		printf("Error (C): unexpected '%c'\n", str[i]);
		exit(1);
	}
}

void	parse_light(t_win *win, char *elems_str, char *str, int i)
{
	float	light_ratio;

	if (elems_str[(int)str[i]])
	{
		printf("Error: light already exists\n");
		exit(1);
	}
	elems_str[(int)str[i++]] = 1;
	win->scene.light.x = parse_number(str, &i);
	i = skip(str, ',', i);
	win->scene.light.y = parse_number(str, &i);
	i = skip(str, ',', i);
	win->scene.light.z = parse_number(str, &i);
	light_ratio = parse_number(str, &i);
	parse_error(light_ratio, 0, 1, " light brightness");
	if (str[i])
	{
		printf("Error (L): unexpected '%c'\n", str[i]);
		exit(1);
	}
}
