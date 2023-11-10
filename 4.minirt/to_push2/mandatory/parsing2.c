/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:51:01 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:11:01 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	parse_sphere(t_win *win, char *str, int i)
{
	t_obj	obj;

	i += 2;
	obj.type = sphere_;
	obj.center.x = parse_number(str, &i);
	i = skip(str, ',', i);
	obj.center.y = parse_number(str, &i);
	i = skip(str, ',', i);
	obj.center.z = parse_number(str, &i);
	obj.radius = parse_number(str, &i) / 2;
	obj.color = parse_color(str, &i);
	add_objects(win, obj);
	if (str[i] || obj.radius <= 0)
		error_exit(0);
}

void	parse_plan(t_win *win, char *str, int i)
{
	t_obj	obj;
	t_vec3	point;

	i += 2;
	obj.type = plan_;
	point.x = parse_number(str, &i);
	i = skip(str, ',', i);
	point.y = parse_number(str, &i);
	i = skip(str, ',', i);
	point.z = parse_number(str, &i);
	obj.normal = parse_normal(str, &i);
	obj.d = -dot(point, obj.normal);
	obj.color = parse_color(str, &i);
	add_objects(win, obj);
	if (str[i])
		error_exit(0);
}

void	parse_cylinder(t_win *win, char *str, int i)
{
	t_obj	obj;

	i += 2;
	obj.type = cylinder_;
	obj.center.x = parse_number(str, &i);
	i = skip(str, ',', i);
	obj.center.y = parse_number(str, &i);
	i = skip(str, ',', i);
	obj.center.z = parse_number(str, &i);
	obj.normal = parse_normal(str, &i);
	obj.radius = parse_number(str, &i) / 2;
	obj.height = parse_number(str, &i);
	obj.color = parse_color(str, &i);
	add_objects(win, obj);
	if (str[i] || obj.radius <= 0 || obj.height <= 0)
		error_exit(0);
}

void	parse_line(t_win *win, char *str, char *elems_str)
{
	int	i;

	i = 0;
	while (str && str[i])
	{
		if (str[i] && ft_isspace(str[i]))
			i++;
		else if (str[i] == 'A')
			return (parse_ambient_light(win, elems_str, str, i));
		else if (str[i] == 'C')
			return (parse_camera(win, elems_str, str, i));
		else if (str[i] == 'L')
			return (parse_light(win, elems_str, str, i));
		else if (ft_strstr(str + i, "sp") == str + i)
			return (parse_sphere(win, str, i));
		else if (ft_strstr(str + i, "pl") == str + i)
			return (parse_plan(win, str, i));
		else if (ft_strstr(str + i, "cy") == str + i)
			return (parse_cylinder(win, str, i));
		else
			error_exit(0);
	}
}

void	parse_file(t_win *win, int argc, char **argv)
{
	char	elems_str[255];
	int		fd;
	char	*str;

	ft_memset(elems_str, 0, sizeof(elems_str));
	if (argc != 2 || ft_strlen(argv[1]) < 3 || ft_strcmp(argv[1]
			+ ft_strlen(argv[1]) - 3, ".rt"))
		error_exit("Exepcted a \".rt\" file as argument");
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		error_exit("Couldn't open scene file");
	str = ft_readline(fd);
	while (str)
	{
		if (str[0] != '#')
			parse_line(win, str, elems_str);
		free(str);
		str = ft_readline(fd);
	}
	if (!elems_str['C'])
		error_exit("Camera was not defined");
	close(fd);
}
