# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/09 00:44:30 by mhrima            #+#    #+#              #
#    Updated: 2023/11/09 00:46:39 by mhrima           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = ./minirt
CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -rf

SRCS = hit_object0.c main.c math1.c mlx.c parsing1.c raytracing0.c utils0.c utils2.c hit_object1.c math0.c math2.c parsing0.c parsing2.c raytracing1.c utils1.c
OBJS = $(SRCS:.c=.o)

SAN = -fsanitize=address -g3

all: $(NAME)

$(NAME) : $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -lm -lmlx -lXext -lX11 -o $(NAME)

clean : 
	$(RM) $(OBJS) $(OBJS_BONUS)
	
fclean : clean
	$(RM) $(NAME) $(NAME_BONUS)

re : fclean all