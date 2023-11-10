/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zfarini <zfarini@student.1337.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 13:00:18 by zfarini           #+#    #+#             */
/*   Updated: 2023/11/09 12:18:05 by zfarini          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include <mlx.h>
# include <stdio.h>
# include <stdint.h>
# include <math.h>
# include <stddef.h>
# include <stdlib.h>
# include <float.h>
# include <time.h>
# include <assert.h>
# include <fcntl.h>
# include <unistd.h>
# include <limits.h>
# include <time.h>

# define WINDOW_WIDTH 512
# define WINDOW_HEIGHT 512
# define EPSILON 0.0001f
# define MIN_T 0.0001f
# define PI 3.14159265359f
# define BUMP_MAP_STRENGTH 10
# define SPECULAR_POWER 20

# ifdef __APPLE__

enum e_keycodes
{
	KEYCODE_SPACE = 49,
	KEYCODE_W = 13,
	KEYCODE_A = 0,
	KEYCODE_D = 2,
	KEYCODE_S = 1,
	KEYCODE_Q = 12,
	KEYCODE_E = 14,
	KEYCODE_LEFT = 123,
	KEYCODE_RIGHT = 124,
	KEYCODE_UP = 126,
	KEYCODE_DOWN = 125,
	KEYCODE_ESCAPE = 53,
};

# else

enum e_keycodes
{
	KEYCODE_SPACE = 32,
	KEYCODE_W = 119,
	KEYCODE_A = 97,
	KEYCODE_D = 100,
	KEYCODE_S = 115,
	KEYCODE_Q = 113,
	KEYCODE_E = 101,
	KEYCODE_LEFT = 65361,
	KEYCODE_RIGHT = 65363,
	KEYCODE_UP = 65362,
	KEYCODE_DOWN = 65364,
	KEYCODE_ESCAPE = 65307,
};

# endif

typedef struct s_vector
{
	float	x;
	float	y;
	float	z;
}	t_vector;

typedef struct s_matrix
{
	float	e[3][3];
}	t_matrix;

enum e_object_type
{
	OBJECT_NONE,
	OBJECT_SPHERE,
	OBJECT_PLANE,
	OBJECT_CYLINDER,
	OBJECT_CONE,
};

enum e_camera_dir
{
	CAMERA_LEFT,
	CAMERA_RIGHT,
	CAMERA_UP,
	CAMERA_DOWN,
	CAMERA_FORWARD,
	CAMERA_BACKWARD,
};

typedef struct s_texture
{
	int			width;
	int			height;
	int			pitch;
	uint32_t	*pixels;
	void		*image;
}	t_texture;

typedef struct s_object
{
	int			type;
	t_vector	center;
	t_vector	axis;
	float		radius;
	float		height;
	float		d;
	t_vector	color;
	t_texture	*texture;
	float		tex_scale_u;
	float		tex_scale_v;
	t_texture	*bump;
	float		bump_scale_u;
	float		bump_scale_v;
	t_vector	xaxis;
	t_vector	yaxis;
}	t_object;

typedef struct s_light
{
	t_vector	p;
	t_vector	color;
}	t_light;

typedef struct s_minirt
{
	void		*mlx;
	void		*window;
	void		*image;
	int			width;
	int			height;
	uint32_t	*pixels;
	t_object	*objects;
	int			object_count;
	t_vector	ambient_light_color;
	t_vector	camera_scene_dir;
	t_vector	camera_scene_up;
	t_vector	camera_p;
	t_vector	camera_up;
	t_vector	camera_dir;
	t_vector	camera_x;
	t_vector	camera_y;
	t_vector	camera_z;
	t_vector	pixel_du;
	t_vector	pixel_dv;
	t_vector	film_center;
	t_vector	camera_rotation;
	t_vector	camera_dp;
	float		camera_fov;
	float		camera_focal_length;
	float		film_width;
	float		film_height;
	t_light		*lights;
	int			light_count;
	t_vector	*color_sum;
	int			accumulate_frame;
	int			is_mouse_down;
	int			prev_mouse_x;
	int			prev_mouse_y;
	int			frame;
	int			is_pressed[6];
	char		*curr_line;
	int			line_number;
	int			parse_line;
	int			camera_def;
	int			ambient_light_def;
	char		*texture_name;
	int			bytes_read;
	char		*new_line;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
}	t_minirt;

typedef struct s_hit_info
{
	float		t;
	t_object	*object;
	float		u;
	float		v;
	t_vector	normal;
	t_vector	tangent;
	t_vector	bitangent;
	t_vector	p;
	float		a;
	float		b;
	float		c;
	float		d;
	float		delta;
	float		t0;
	float		t1;
	int			open;
	t_vector	c0;
	t_vector	c1;
	t_vector	o;
}	t_hit_info;

t_vector	vector_add(t_vector a, t_vector b);
t_vector	vector_sub(t_vector a, t_vector b);
t_vector	vector_mul(t_vector a, t_vector b);
t_vector	vector_scale(t_vector v, float s);
float		vector_length(t_vector v);
t_vector	vector_normalize(t_vector v);
float		vector_dot(t_vector a, t_vector b);
t_vector	vector_cross(t_vector a, t_vector b);
t_vector	vector_reflect(t_vector a, t_vector n);

float		random_float(float min, float max);
float		square(float x);
float		lerp(float a, float t, float b);
int			imin(int x, int y);
int			imax(int x, int y);
t_vector	get_up_vector(t_vector forward);
t_vector	ray_at(t_vector ray_origin, t_vector ray_dir, float t);
float		float_sign(float x);

t_vector	matrix_mul_vector(t_matrix m, t_vector v);
t_matrix	matrix_mul(t_matrix a, t_matrix b);
t_matrix	matrix_transpose(t_matrix m);
t_matrix	x_rotation(float a);
t_matrix	y_rotation(float a);
t_matrix	z_rotation(float a);

void		load_scene(t_minirt *mrt, char *filename);

void		ft_memset(void *dest, int value, size_t size);
void		ft_memcpy(void *dest, void *src, size_t size);
int			ft_isspace(char c);
size_t		ft_strlen(char *s);
int			ft_strcmp(char *s1, char *s2);
void		*ft_calloc(t_minirt *mrt, size_t size, size_t count);
void		mrt_exit(t_minirt *mrt, char *msg);

float		ray_intersect_plane(t_vector ray_origin, t_vector ray_dir,
				t_vector normal, float d);
float		sample_bilinear(t_texture *b, float u, float v);

t_hit_info	ray_hit_sphere(t_object *e, t_vector ray_origin, t_vector ray_dir);
t_hit_info	ray_hit_plane(t_object *e, t_vector ray_origin, t_vector ray_dir);
t_hit_info	ray_hit_cylinder(t_object *e, t_vector ray_origin,
				t_vector ray_dir);
t_hit_info	ray_hit_cone(t_object *e, t_vector ray_origin, t_vector ray_dir);

int			on_mouse_down(int button, int x, int y, t_minirt *mrt);
int			on_mouse_up(int button, int x, int y, t_minirt *mrt);
int			on_mouse_movement(int x, int y, t_minirt *mrt);
int			on_key_down(int keycode, t_minirt *mrt);
int			on_key_up(int keycode, t_minirt *mrt);

t_vector	raytrace(t_minirt *mrt, t_vector ray_origin, t_vector ray_dir);

float		sample_bump_map(t_object *e, t_texture *b, float u, float v);
void		update_camera(t_minirt *mrt);

int			minirt(t_minirt *mrt);

t_texture	*parse_texture(t_minirt *mrt, char **line,
				float *scale_u, float *scale_v);
char		*get_next_line(t_minirt *mrt, int fd);
void		skip_spaces(char **line);
int			parse_int_value(t_minirt *mrt, char **line, int sign);
int			parse_int(t_minirt *mrt, char **line, int skip);

float		parse_float(t_minirt *mrt, char **line);
t_vector	parse_color(t_minirt *mrt, char **line);

t_vector	parse_vector(t_minirt *mrt, char **line, int normal);
void		parse_object(t_minirt *mrt, char **line);
#endif
