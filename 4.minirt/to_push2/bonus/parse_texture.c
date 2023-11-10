/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_texture.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 12:46:02 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/09 11:58:30 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

t_texture	*load_texture(t_minirt *mrt, char *filename)
{
	int			width;
	int			height;
	void		*image;
	t_texture	*t;

	image = mlx_xpm_file_to_image(mrt->mlx, filename, &width, &height);
	if (!image)
	{
		printf("Error\nFailed to load texture \"%s\"\n", filename);
		mrt_exit(mrt, 0);
	}
	t = ft_calloc(mrt, 1, sizeof(t_texture));
	t->width = width;
	t->height = height;
	t->pixels = (uint32_t *)mlx_get_data_addr(image,
			&mrt->bits_per_pixel, &mrt->line_length, &mrt->endian);
	t->image = image;
	if (!t->pixels)
	{
		printf("Error\nFailed to load texture \"%s\"\n", filename);
		mrt_exit(mrt, 0);
	}
	return (t);
}

char	*parse_word(t_minirt *mrt, char **line)
{
	int		length;
	char	*word;

	skip_spaces(line);
	length = 0;
	while ((*line)[length] && !ft_isspace((*line)[length])
			&& (*line)[length] != ',')
		length++;
	if (!length)
		return (0);
	word = ft_calloc(mrt, length + 1, 1);
	length = 0;
	while (**line && !ft_isspace(**line) && **line != ',')
	{
		word[length++] = **line;
		*line = *line + 1;
	}
	return (word);
}

void	parse_texture_scale(t_minirt *mrt, char **line,
		float *scale_u, float *scale_v)
{
	free(mrt->texture_name);
	mrt->texture_name = 0;
	*scale_u = 1;
	*scale_v = 1;
	skip_spaces(line);
	if ((*line)[0] == ',')
	{
		*line = *line + 1;
		*scale_u = parse_float(mrt, line);
		skip_spaces(line);
		if ((*line)[0] != ',')
			mrt_exit(mrt, "Parse Error");
		*line = *line + 1;
		*scale_v = parse_float(mrt, line);
		if (*scale_u < 1 || *scale_v < 1)
			mrt_exit(mrt, "Expected texture scale values to be bigger than 1");
	}
}

t_texture	*parse_texture(t_minirt *mrt, char **line,
		float *scale_u, float *scale_v)
{
	t_texture	*texture;

	mrt->texture_name = parse_word(mrt, line);
	if (!mrt->texture_name || !ft_strcmp(mrt->texture_name, "none"))
	{
		free(mrt->texture_name);
		mrt->texture_name = 0;
		return (0);
	}
	if (!ft_strcmp(mrt->texture_name, "checkerboard"))
	{
		texture = ft_calloc(mrt, 1, sizeof(t_texture));
		texture->pixels = ft_calloc(mrt, 4, sizeof(uint32_t));
		texture->width = 2;
		texture->height = 2;
		texture->pixels[0] = 0xffffff;
		texture->pixels[3] = 0xffffff;
		texture->pixels[1] = 0x222222;
		texture->pixels[2] = 0x222222;
	}
	else
		texture = load_texture(mrt, mrt->texture_name);
	parse_texture_scale(mrt, line, scale_u, scale_v);
	return (texture);
}
