#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct s_vec3
{
    float x;
    float y;
    float z;
} t_vec3;

typedef struct s_pars
{
    char id;
    t_vec3 coors;
    float bright;
    t_vec3 color;
    t_vec3 dir;
    float fov;
    float diam;
    t_vec3 norm;
    float height;
} t_pars;

void *ft_calloc(int len)
{
    char *ptr;
    int i;

    ptr = malloc(len);
    if (ptr == NULL)
    {
        printf("Error allocation");
        exit(-1);
    }
    i = 0;
    while (i < len)
        ptr[i++] = 0;
    return ptr;
}

void ft_memcpy(void *src, void *dst, int size)
{
    int i = 0;
    while (i < size)
    {
        ((char *)src)[i] = ((char *)dst)[i];
        i++;
    }
}

char *join(char *str, char c)
{
    char *tmp;
    static int i;

    if (str == NULL)
        str = ft_calloc(100);
    if (!(i + 1) % 100)
    {
        tmp = ft_calloc((i + 1) + 100);
        ft_memcpy(str, tmp, i);
        free(str);
        str = tmp;
    }
    if (c == 0 || c == '\n')
        i = 0;
    else
        str[i++] = c;
    return str;
}

char *readline(int fd)
{
    char c;
    int r;
    char *str;

    c = 0;
    str = NULL;
    while (1)
    {
        r = read(fd, &c, 1);
        if (r == 0)
            break;
        str = join(str, c);
        if (c == 0 || c == '\n')
            break;
    }
    return str;
}

char *ft_strchr(char *str, char c)
{
    int i;

    if (!str)
        return NULL;
    i = 0;
    while (str[i] && str[i] != c)
        i++;
    return (str + i);
}

t_pars prase(char *line)
{
    t_pars pars;

    int i = 0;
    while (line[i])
    {
        if (ft_strchr("CL", line[0]))
            i++;
        else
            i++;
    }
}

int main(void)
{
    t_pars list[100];
    int fd = open("map.rt", O_RDONLY);
    if (fd < 0)
        printf("Error: file dosn't exist\n");
    while (1)
    {
        char *str = readline(fd);
        if (!str)
            break;
        printf("> %s\n", str);
        free(str);
    }
}