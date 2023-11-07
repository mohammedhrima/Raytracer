#include "Libft/libft.h"

typedef struct s_vec3
{
    float x;
    float y;
    float z;
} t_vec3;

typedef struct s_elm
{
    char id;
    float ratio;
    t_vec3 color;
    t_vec3 point;
    t_vec3 normal;
    float fov;
    float diameter;
    float height;
} t_elem;

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

bool in_range(float num, float min, float max, char *msg)
{
    return (num >= min && num <= max);
}

t_vec3 parse_color(char *str, int *i)
{
    t_vec3 color;
    color.x = parse_number(str, i);
    *i = skip(str, ',', *i);
    color.y = parse_number(str, i);
    *i = skip(str, ',', *i);
    color.z = parse_number(str, i);
    if (!in_range(color.x, 0, 255) || !in_range(color.y, 0, 255) || !in_range(color.z, 0, 255))
    {
        printf("Error: RGB should be in range [0,255]\n");
        exit(1);
    }
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
    if (!in_range(normal.x, -1, 1) || !in_range(normal.y, -1, 1) || !in_range(normal.z, -1, 1))
    {
        printf("Error: normal should have values in range [-1,1]\n");
        exit(1);
    }
    return normal;
}

t_elem parse_ambient_light(char *str, int i)
{
    t_elem elem = (t_elem){0};
    elem.id = str[i++];
    elem.ratio = parse_number(str, &i);
    elem.color = parse_color(str, &i);
    if (str[i])
    {
        printf("Error 2 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("elem is %c, %f, %f, %f, %f\n", elem.id, elem.ratio, elem.color.x, elem.color.y, elem.color.z);
    return elem;
}

t_elem parse_camera(char *elems_str, char *str, int i)
{
    t_elem elem = (t_elem){0};
    if (elems_str[str[i]])
    {
        free(elems_str);
        printf("Error: camera already exists\n");
        exit(1);
    }
    elems_str[str[i]] = 1;
    elem.id = str[i++];
    elem.point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.z = parse_number(str, &i);
    elem.normal = parse_normal(str, &i);
    elem.fov = parse_number(str, &i);
    if (str[i])
    {
        free(elems_str);
        printf("Error 3 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("elem is %c, %f, %f, %f  %f, %f, %f  %f\n", elem.id, elem.point.x, elem.point.y, elem.point.z, elem.normal.x, elem.normal.y, elem.normal.z, elem.fov);
    return elem;
}

t_elem parse_light(char *elems_str, char *str, int i)
{
    t_elem elem = (t_elem){0};
    if (elems_str[str[i]])
    {
        free(elems_str);
        printf("Error: light already exists\n");
        exit(1);
    }
    elems_str[str[i]] = 1;
    elem.id = str[i++];
    elem.point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.z = parse_number(str, &i);
    elem.ratio = parse_number(str, &i);
    elem.color = parse_color(str, &i);
    if (str[i])
    {
        free(elems_str);
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("elem is %c, %f, %f, %f  %f %f, %f, %f \n", elem.id, elem.point.x, elem.point.y, elem.point.z, elem.ratio, elem.color.x, elem.color.y, elem.color.z);
    return elem;
}

t_elem parse_sphere(char *elems_str, char *str, int i)
{
    i += 2;
    t_elem elem = (t_elem){0};
    elems_str['s']++;
    elem.id = 's';
    elem.point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.z = parse_number(str, &i);
    elem.diameter = parse_number(str, &i);
    elem.color = parse_color(str, &i);
    if (str[i])
    {
        free(elems_str);
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("elem is %c, %f, %f, %f  %f %f, %f, %f \n", elem.id, elem.point.x, elem.point.y, elem.point.z, elem.ratio, elem.color.x, elem.color.y, elem.color.z);
    return elem;
}

t_elem parse_plan(char *elems_str, char *str, int i)
{
    i += 2;
    t_elem elem = (t_elem){0};
    elems_str['p']++;
    elem.id = 'p';
    elem.point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.z = parse_number(str, &i);
    elem.normal = parse_normal(str, &i);
    elem.color = parse_color(str, &i);
    if (str[i])
    {
        free(elems_str);
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("elem is %c, %f, %f, %f  %f, %f, %f %f, %f, %f \n", elem.id, elem.point.x, elem.point.y, elem.point.z, elem.normal.x, elem.normal.y, elem.normal.z, elem.color.x, elem.color.y, elem.color.z);
    return elem;
}

t_elem parse_cylinder(char *elems_str, char *str, int i)
{
    i += 2;
    t_elem elem = (t_elem){0};
    elems_str['c']++;
    elem.id = 'c';
    elem.point.x = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.y = parse_number(str, &i);
    i = skip(str, ',', i);
    elem.point.z = parse_number(str, &i);
    elem.normal = parse_normal(str, &i);
    elem.diameter = parse_number(str, &i);
    elem.height = parse_number(str, &i);
    elem.color = parse_color(str, &i);
    if (str[i])
    {
        free(elems_str);
        printf("Error 4 unexpected '%c'\n", str[i]);
        exit(1);
    }
    printf("elem is %c, %f, %f, %f  %f, %f, %f  %f %f  %f, %f, %f \n", elem.id, elem.point.x, elem.point.y, elem.point.z, elem.normal.x, elem.normal.y, elem.normal.z,
           elem.diameter, elem.height,
           elem.color.x, elem.color.y, elem.color.z);
    return elem;
}

t_elem parse_line(char *str, char *elems_str)
{
    int i = 0;
    int require = 0;
    t_elem elem = (t_elem){0};

    while (str && str[i])
    {
        while (str[i] && ft_isspace(str[i]))
            i++;
        if (str[i] == 'A')
            return parse_ambient_light(str, i);
        else if (str[i] == 'C')
            return parse_camera(elems_str, str, i);
        else if (str[i] == 'L')
            return parse_light(elems_str, str, i);
        else if (ft_strstr(str + i, "sp") == str + i)
            return parse_sphere(elems_str, str, i);
        else if (ft_strstr(str + i, "pl") == str + i)
            return parse_plan(elems_str, str, i);
        else if (ft_strstr(str + i, "cy") == str + i)
            return parse_cylinder(elems_str, str, i);
        else
        {
            printf("Error\n");
            exit(1);
        }
    }

    return elem;
}

int main(int argc, char **argv)
{
    int fd = get_file(argc, argv);
    char *str = ft_readline(fd);
    t_elem elems[100];
    char *elems_str;
    elems_str = ft_calloc(255, 1);
    int pos = 0;
    while (str)
    {
        printf("parse :%s\n\n", str);
        parse_line(str, elems_str);
        str = ft_readline(fd);
    }
    free(elems_str);
}