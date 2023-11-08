#include "utils.c"

int get_file(int argc, char **argv)
{
    if (argc != 2 || ft_strlen(argv[1]) < 3 || ft_strcmp(argv[1] + ft_strlen(argv[1]) - 3, ".rt"))
    {
        printf("executable require .rt file as argument\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        printf("Error: opening file\n");
        exit(1);
    }
    return fd;
}

int skip(char *str, char c, int i)
{
    if (str[i] == '\0')
    {
        printf("Error unexpected EOL\n");
        exit(1);
    }
    if (str[i] != c)
    {
        printf("Error 0 unexpected '%c'\n", str[i]);
        exit(1);
    }
    i++;
    if (str[i] == '\0')
    {
        printf("Error unexpected EOL\n");
        exit(1);
    }
    return i;
}

float parse_number(char *str, int *ptr)
{
    int i = *ptr;
    float res = 0.0;
    int sign = 1;

    while (str[i] && ft_isspace(str[i]))
        i++;
    if (ft_strchr("+-", str[i]))
    {
        if (str[i] == '-')
            sign = -1;
        i = skip(str, str[i], i);
    }
    if (!ft_isdigit(str[i]))
    {
        printf("Error 1 unexpected '%c'\n", str[i]);
        exit(1);
    }
    while (ft_isdigit(str[i]))
    {
        res = 10 * res + sign * (str[i] - '0');
        i++;
    }
    if (str[i] == '.')
    {
        i++;
        float j = 0.1;
        while (ft_isdigit(str[i]))
        {
            res = res + sign * j * (str[i] - '0');
            j /= 10;
            i++;
        }
    }
    while (str[i] && ft_isspace(str[i]))
        i++;
    *ptr = i;
    return res;
}

void parse_error(float num, float min, float max, char *msg)
{
    if (num < min || num > max)
    {
        printf("Error: %s should be in range [%f,%f]\n", msg, min, max);
        exit(1);
    }
}

t_vec3 parse_color(char *str, int *i)
{
    t_vec3 color;
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
    return color;
}

t_vec3 parse_normal(char *str, int *i)
{
    t_vec3 normal;

    normal.x = parse_number(str, i);
    *i = skip(str, ',', *i);
    normal.y = parse_number(str, i);
    *i = skip(str, ',', *i);
    normal.z = parse_number(str, i);
    parse_error(normal.x, -1, 1, "normal");
    parse_error(normal.y, -1, 1, "normal");
    parse_error(normal.z, -1, 1, "normal");
    return normal;
}

void parse_ambient_light(t_win *win, char *elems_str, char *str, int i)
{
    if (elems_str[str[i]])
    {
        // free(elems_str);
        printf("Error: ambient light already exists\n");
        exit(1);
    }
    elems_str[str[i++]] = 1;
    win->scene.ambient_light_ratio = parse_number(str, &i);
    parse_error(win->scene.ambient_light_ratio, 0, 1, "ambient light ratio");
    win->scene.ambient_light_color = parse_color(str, &i);
    if (str[i])
    {
        printf("Error 2 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("A , %f, %f, %f, %f\n", win->scene.light_ratio, win->scene.ambient_light_color.x,
           win->scene.ambient_light_color.y, win->scene.ambient_light_color.z);
}

void parse_camera(t_win *win, char *elems_str, char *str, int i)
{
    if (elems_str[str[i]])
    {
        // free(elems_str);
        printf("Error: camera already exists\n");
        exit(1);
    }
    elems_str[str[i++]] = 1;
    win->scene.camera.x = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.camera.y = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.camera.z = parse_number(str, &i);
    win->scene.cam_dir = parse_normal(str, &i);
    win->scene.fov = parse_number(str, &i);
    win->scene.focal_len = length(win->scene.camera);
    parse_error(win->scene.fov, 0, 180, "fov");
    if (str[i])
    {
        // free(elems_str);
        printf("Error 3 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("C %f, %f, %f  %f, %f, %f  %f\n", win->scene.camera.x, win->scene.camera.y,
           win->scene.camera.z, win->scene.cam_dir.x, win->scene.cam_dir.y,
           win->scene.cam_dir.z, win->scene.fov);
}

void parse_light(t_win *win, char *elems_str, char *str, int i)
{
    if (elems_str[str[i]])
    {
        // free(elems_str);
        printf("Error: light already exists\n");
        exit(1);
    }
    elems_str[str[i++]] = 1;
    win->scene.light.x = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.light.y = parse_number(str, &i);
    i = skip(str, ',', i);
    win->scene.light.z = parse_number(str, &i);
    win->scene.light_ratio = parse_number(str, &i);
    parse_error(win->scene.light_ratio, 0, 1, " light brightness");
    win->scene.light_color = parse_color(str, &i);

    if (str[i])
    {
        // free(elems_str);
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("L, %f, %f, %f  %f %f, %f, %f \n", win->scene.light.x, win->scene.light.y,
           win->scene.light.z, win->scene.light_ratio, win->scene.light_color.x, win->scene.light_color.y,
           win->scene.light_color.z);
}

void parse_sphere(t_win *win, char *str, int i)
{
    t_vec3 center;
    i += 2;
    center.x = parse_number(str, &i);
    i = skip(str, ',', i);
    center.y = parse_number(str, &i);
    i = skip(str, ',', i);
    center.z = parse_number(str, &i);
    add_objects(win, new_sphere(center, parse_number(str, &i) / 2, parse_color(str, &i)));

    if (str[i])
    {
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("sp, %f, %f, %f  %f %f, %f, %f \n", center.x, center.y, center.z,
           win->scene.objects[win->scene.pos - 1].radius * 2,
           win->scene.objects[win->scene.pos - 1].color.x,
           win->scene.objects[win->scene.pos - 1].color.y,
           win->scene.objects[win->scene.pos - 1].color.z);
}

void parse_plan(t_win *win, char *str, int i)
{
    t_vec3 point;
    t_vec3 normal;
    i += 2;
    point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    point.z = parse_number(str, &i);
    normal = parse_normal(str, &i);
    add_objects(win, new_plan(normal, unit_dot(point, normal), parse_color(str, &i)));
    if (str[i])
    {
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("pl, %f, %f, %f  %f, %f, %f %f, %f, %f \n", point.x, point.y, point.z, normal.x, normal.y, normal.z,
           win->scene.objects[win->scene.pos - 1].color.x,
           win->scene.objects[win->scene.pos - 1].color.y,
           win->scene.objects[win->scene.pos - 1].color.z);
}

void parse_cylinder(t_win *win, char *str, int i)
{
    i += 2;
    t_vec3 point;

    point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    point.z = parse_number(str, &i);
    add_objects(win, new_cylinder(point, parse_normal(str, &i), parse_number(str, &i), parse_number(str, &i), parse_color(str, &i)));
    if (str[i])
    {
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("cy, %f, %f, %f  %f, %f, %f  %f %f  %f, %f, %f \n", point.x, point.y, point.z,
           win->scene.objects[win->scene.pos - 1].normal.x, win->scene.objects[win->scene.pos - 1].normal.y,
           win->scene.objects[win->scene.pos - 1].normal.z, win->scene.objects[win->scene.pos - 1].radius * 2,
           win->scene.objects[win->scene.pos - 1].height, win->scene.objects[win->scene.pos - 1].color.x,
           win->scene.objects[win->scene.pos - 1].color.y, win->scene.objects[win->scene.pos - 1].color.z);
}

void parse_line(t_win *win, char *str, char *elems_str)
{
    int i = 0;
    int require = 0;

    while (str && str[i])
    {
        while (str[i] && ft_isspace(str[i]))
            i++;
        if (str[i] == 'A')
            return parse_ambient_light(win, elems_str, str, i);
        else if (str[i] == 'C')
            return parse_camera(win, elems_str, str, i);
        else if (str[i] == 'L')
            return parse_light(win, elems_str, str, i);
        else if (ft_strstr(str + i, "sp") == str + i)
            return parse_sphere(win, str, i);
        else if (ft_strstr(str + i, "pl") == str + i)
            return parse_plan(win, str, i);
        else if (ft_strstr(str + i, "cy") == str + i)
            return parse_cylinder(win, str, i);
        else
        {
            printf("Error\n");
            exit(1);
        }
    }
}

// TODOS:
// parse focal len
// parse fov
// use a ft_exit that fress everything
// light_ratio ???
// use light color
// ambient light maybe it's background color
// remove all frees
int main(int argc, char **argv)
{
    int fd = get_file(argc, argv);
    char *str = ft_readline(fd);
    char *elems_str;
    elems_str = ft_calloc(255, 1);
    t_win *win;
    win = new_window(HEIGHT, WIDTH);

    while (str)
    {
        printf("parse :%s\n\n", str);
        parse_line(win, str, elems_str);
        str = ft_readline(fd);
    }
    win->scene.light = (t_vec3){10, 10, 10};
    win->scene.camera = (t_vec3){.x = 0, .y = 0, .z = FOCAL_LEN};
    win->scene.cam_dir = (t_vec3){.x = 0, .y = 0, .z = -FOCAL_LEN};
    
    init_scene(win);
    set_pixels(win);
    mlx_hook(win->win, 2, 1L << 0, listen_on_key, win);
    mlx_loop_hook(win->mlx, draw, win);
    mlx_loop(win->mlx);

    close_window(win);
}