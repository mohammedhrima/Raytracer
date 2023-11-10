/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 10:54:48 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/09 13:58:42 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	parse_ambient_light(t_minirt *mrt, char **line)
{
	float	ratio;

	if (mrt->ambient_light_def)
		mrt_exit(mrt, "Ambient light was defined multiple times");
	*line = *line + 1;
	ratio = parse_float(mrt, line);
	if (ratio < 0 || ratio > 1)
		mrt_exit(mrt, "Expected ambient light ratio in range [0-1]");
	mrt->ambient_light_color = vector_scale(parse_color(mrt, line), ratio);
	mrt->ambient_light_def = 1;
}

void	parse_camera(t_minirt *mrt, char **line)
{
	if (mrt->camera_def)
		mrt_exit(mrt, "Camera was definied multiple times");
	*line = *line + 1;
	mrt->camera_p = parse_vector(mrt, line, 0);
	mrt->camera_scene_dir = parse_vector(mrt, line, 1);
	mrt->camera_scene_up = get_up_vector(mrt->camera_scene_dir);
	mrt->camera_fov = parse_float(mrt, line);
	if (mrt->camera_fov < 0 || mrt->camera_fov > 180)
		mrt_exit(mrt, "Expected camera fov in range [0-180]");
	mrt->camera_def = 1;
}

void	parse_light(t_minirt *mrt, char **line)
{
	float		ratio;
	t_vector	color;
	t_vector	p;
	t_light		*new_lights;

	*line = *line + 1;
	new_lights = ft_calloc(mrt, mrt->light_count + 1, sizeof(t_light));
	ft_memcpy(new_lights, mrt->lights, mrt->light_count * sizeof(t_light));
	p = parse_vector(mrt, line, 0);
	ratio = parse_float(mrt, line);
	if (ratio < 0 || ratio > 1)
		mrt_exit(mrt, "Light ratio is not in range [0-1]");
	color = vector_scale(parse_color(mrt, line), ratio);
	new_lights[mrt->light_count++] = (t_light){p, color};
	free(mrt->lights);
	mrt->lights = new_lights;
}

void	parse_line(t_minirt *mrt, char *line)
{
	if (line[0] == '#')
		return ;
	skip_spaces(&line);
	if (!line[0])
		return ;
	if (line[0] == 'A' && ft_isspace(line[1]))
		parse_ambient_light(mrt, &line);
	else if (line[0] == 'C' && ft_isspace(line[1]))
		parse_camera(mrt, &line);
	else if (line[0] == 'L' && ft_isspace(line[1]))
		parse_light(mrt, &line);
	else
		parse_object(mrt, &line);
	skip_spaces(&line);
	if (line[0])
		mrt_exit(mrt, "Parse error");
	mrt->parse_line++;
}

void	load_scene(t_minirt *mrt, char *filename)
{
	int		fd;
	char	*line;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		mrt_exit(mrt, "Couldn't open the scene file");
	while (1)
	{
		free(mrt->curr_line);
		line = get_next_line(mrt, fd);
		if (!line)
			break ;
		parse_line(mrt, line);
	}
	if (!mrt->camera_def)
		mrt_exit(mrt, "Camera was not defined");
	close(fd);
}
