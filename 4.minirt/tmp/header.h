/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/29 10:13:59 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 04:04:00 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
# define HEADER_H

# include <float.h>
# include <limits.h>
# include <math.h>
# include <mlx.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>

# define ZERO .0001f
# define WIDTH 400
# define HEIGHT WIDTH/1.5f
# define ESC 65307
// # define FOCAL_LEN 10
# define RAYS_PER_PIXEL 25
# define PI 3.1415926535897932385

typedef enum s_type
{
	zero_,
	sphere_,
	plan_,
	cylinder_,
	cone_,
}				t_type;

typedef struct s_vec3
{
	float		x;
	float		y;
	float		z;
}				t_vec3;
typedef t_vec3	t_color;

typedef struct s_ray
{
	t_vec3		dir;
	t_vec3		org;
}				t_ray;



typedef struct s_obj
{
	t_type		type;
	t_color		color;
	t_vec3		normal;
	float		d;
	t_vec3		center;
    float		radius;
	float		height;
}				t_obj;

typedef struct s_scene
{
	t_vec3		light;
	t_vec3		camera;
	t_vec3		cam_dir;
	t_vec3		pixel_u;
	t_vec3		pixel_v;
	t_vec3		first_pixel;
	t_obj		*objects;
	int			pos;
	t_vec3		screen_u;
	t_vec3		screen_v;
	t_vec3		u;
	t_vec3		v;
	t_vec3		w;
	t_vec3		up;
	t_vec3		center;
	t_vec3		upper_left;
	float		screen_width;
	float		screen_height;
}				t_scene;

typedef struct s_win
{
	int			width;
	int			height;
	void		*mlx;
	void		*win;
	void		*img;
	void		*title;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
	t_scene		scene;
}				t_win;

typedef struct s_equ
{
	float		a;
	float		b;
	float		c;
	float		delta;
	float		sol;
}				t_equ;

void			*ft_calloc(size_t count, size_t size);
t_vec3			calc(t_vec3 l, char c, t_vec3 r);
t_vec3			scale(t_vec3 v, char c, float t);
float			length_squared(t_vec3 v);
float			length(t_vec3 v);
float			unit_dot(t_vec3 u, t_vec3 v);
t_vec3			unit_vector(t_vec3 v);
t_vec3			random_unit_vector(void);
t_vec3			point_at(t_ray *ray, float t);
t_color			ray_color(t_win *win, t_ray ray);
float			degrees_to_radians(float degrees);
float			random_float(float min, float max);
void			add_objects(t_win *win);
void			set_pixels(t_win *win);
void			close_window(t_win *win);
int				listen_on_key(int code, t_win *win);
int				draw(void *ptr);
t_win			*new_window(int height, int width);
void			set_pixels(t_win *win);
float			hit_sphere(t_ray *ray, t_obj sphere, float min, float max);
float hit_plan(t_ray *ray, t_obj plan, float min, float max);

#endif