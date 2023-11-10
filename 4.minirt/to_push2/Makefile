SRCS = hit_object0.c main.c math1.c mlx.c parsing1.c \
	raytracing0.c utils0.c utils2.c hit_object1.c math0.c \
	math2.c parsing0.c parsing2.c raytracing1.c utils1.c
BSRCS = main.c minirt.c raytrace.c vector.c vector2.c matrix.c parser.c \
	utils.c math_utils.c sphere.c plane.c cylinder.c cone.c input.c camera.c \
	parse_utils.c parse_utils2.c parse_texture.c parse_objects.c math_utils2.c utils2.c

SRC		= $(addprefix mandatory/, ${SRCS})
BSRC	= $(addprefix bonus/, ${BSRCS})
OBJS	= ${SRC:.c=.o}
BOBJS	= ${BSRC:.c=.o}

NAME    = minirt
CC		= cc
CFLAGS  =  -Wall -Wextra -Werror
LDFLAGS = -lmlx -lXext -lX11 -lm
RM		= rm -f

all: ${NAME}

${NAME}: ${OBJS} 
	${CC} ${OBJS} ${LDFLAGS} ${CFLAGS} -o ${NAME}

clean:
	${RM} ${OBJS} ${BOBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all

bonus: $(BOBJS)
	${CC} ${BOBJS} ${LDFLAGS} ${CFLAGS} -o ${NAME}

.PHONY: bonus all clean fclean re
