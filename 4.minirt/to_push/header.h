/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:09:26 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:24:28 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
# define HEADER_H
# include <fcntl.h>
# include <float.h>
# include <limits.h>
# include <math.h>
# include <mlx.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# define ZERO .0001f
# define WIDTH 400
# define HEIGHT WIDTH
# define ESC 65307
# define RAYS_PER_PIXEL 25
# define PI 3.1415926535897932385

typedef enum s_type
{
	sphere_,
	plan_,
	cylinder_,
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
	float		focal_len;
	t_vec3		light;
	t_vec3		light_color;
	t_vec3		ambient_light_color;
	t_vec3		camera;
	t_vec3		cam_dir;
	float		fov;
	t_vec3		pixel_u;
	t_vec3		pixel_v;
	t_vec3		first_pixel;
	t_obj		*objects;
	int			obj_len;
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
	t_vec3		*hit_normal;
}				t_equ;

void			close_window(t_win *win);
int				listen_on_key(int code, t_win *win);
int				draw(void *ptr);
void			add_objects(t_win *win, t_obj obj);
t_win			*new_window(int height, int width);
float			pow2(float x);
float			random_float(float min, float max);
t_vec3			point_at(t_ray *ray, float t);
t_vec3			calc(t_vec3 l, char c, t_vec3 r);
t_vec3			scale(t_vec3 v, char c, float t);
t_vec3			cross(t_vec3 u, t_vec3 v);
t_vec3			unit_vector(t_vec3 v);
float			degrees_to_radians(float degrees);
float			length(t_vec3 v);
float			length_squared(t_vec3 v);
float			dot(t_vec3 u, t_vec3 v);
void			close_window(t_win *win);
int				listen_on_key(int code, t_win *win);
int				draw(void *ptr);
void			parse_file(t_win *win, int argc, char **argv);
void			init_scene(t_win *win);
void			set_pixels(t_win *win);
void			parse_error(float num, float min, float max, char *msg);
float			parse_number(char *str, int *ptr);
int				skip(char *str, char c, int i);
t_vec3			parse_color(char *str, int *i);
t_vec3			parse_normal(char *str, int *i);
void			parse_ambient_light(t_win *win, char *elems_str, char *str,
					int i);
void			parse_camera(t_win *win, char *elems_str, char *str, int i);
void			parse_light(t_win *win, char *elems_str, char *str, int i);
float			hit_sphere(t_ray *ray, t_obj sphere, float max);
float			hit_plan(t_ray *ray, t_obj plan, float max);
float			hit_cylinder(t_ray *ray, t_obj cylin, float max,
					t_vec3 *hit_normal);
t_color			ray_color(t_win *win, t_ray ray);
t_color			check_pixel(t_color pixel);
void			*memory(size_t size);
void			*ft_memcpy(void *destination, void *source, size_t len);
bool			ft_isdigit(int c);
char			*ft_strchr(char *string, int c);
bool			ft_isspace(int c);
char			*ft_strstr(char *from, char *to_find);
size_t			ft_strlen(char *string);
int				ft_strcmp(char *string1, char *string2);
void			ft_strcpy(char *destination, char *source);
char			*ft_charjoin(char *string, char c);
char			*ft_readline(int fd);

#endif