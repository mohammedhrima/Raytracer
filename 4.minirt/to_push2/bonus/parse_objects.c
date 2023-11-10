/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:47:31 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/08 12:47:45 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

void	parse_cone_or_cylinder(t_minirt *mrt, char **line, t_object *e)
{
	if (**line == 'c' && ((*line)[1] == 'y' || (*line)[1] == 'n') &&
			ft_isspace((*line)[2]))
	{
		e->type = OBJECT_CYLINDER;
		if ((*line)[1] == 'n')
			e->type = OBJECT_CONE;
		*line = *line + 2;
		e->center = parse_vector(mrt, line, 0);
		e->axis = parse_vector(mrt, line, 1);
		e->radius = parse_float(mrt, line) * 0.5f;
		e->height = parse_float(mrt, line);
	}
}

void	parse_sphere(t_minirt *mrt, char **line, t_object *e)
{
	if (**line == 's' && (*line)[1] == 'p' && ft_isspace((*line)[2]))
	{
		*line = *line + 2;
		e->type = OBJECT_SPHERE;
		e->center = parse_vector(mrt, line, 0);
		e->radius = parse_float(mrt, line) * 0.5f;
	}
}

void	parse_plane(t_minirt *mrt, char **line, t_object *e)
{
	t_vector	p;

	if (**line == 'p' && (*line)[1] == 'l' && ft_isspace((*line)[2]))
	{
		*line = *line + 2;
		p = parse_vector(mrt, line, 0);
		e->type = OBJECT_PLANE;
		e->axis = parse_vector(mrt, line, 1);
		e->d = -vector_dot(p, e->axis);
	}
}

void	parse_object(t_minirt *mrt, char **line)
{
	t_object	*new_objects;
	t_object	*e;

	new_objects = ft_calloc(mrt, mrt->object_count + 1, sizeof(t_object));
	ft_memcpy(new_objects, mrt->objects, mrt->object_count * sizeof(t_object));
	free(mrt->objects);
	mrt->objects = new_objects;
	e = &new_objects[mrt->object_count++];
	e->radius = 1;
	e->height = 1;
	parse_sphere(mrt, line, e);
	parse_plane(mrt, line, e);
	parse_cone_or_cylinder(mrt, line, e);
	if (!e->type)
		mrt_exit(mrt, "Parse error");
	e->xaxis = vector_normalize(vector_cross(get_up_vector(e->axis), e->axis));
	e->yaxis = vector_normalize(vector_cross(e->axis, e->xaxis));
	e->color = parse_color(mrt, line);
	e->texture = parse_texture(mrt, line, &e->tex_scale_u, &e->tex_scale_v);
	e->bump = parse_texture(mrt, line, &e->bump_scale_u, &e->bump_scale_v);
	if (e->radius <= 0 || e->height <= 0)
		mrt_exit(mrt, "Expected object diameter/height to be positive");
}
