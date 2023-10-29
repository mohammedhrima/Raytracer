# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/29 08:49:10 by mhrima            #+#    #+#              #
#    Updated: 2023/10/29 12:12:36 by mhrima           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -rf

SRCS = main.c mlx.c object_hit.c objects.c pixels.c traceray.c utils.c vectors0.c vectors1.c
OBJS = $(SRCS:.c=.o)

NAME = ./minirt
NAME_BONUS = ./minirt_bonus

SAN = -fsanitize=address -g3

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJS) header.h
	$(CC) $(CFLAGS) $(SAN) $(OBJS) $(LIB) -lmlx -lm -framework OpenGL -framework AppKit -o $(NAME)

$(NAME_BONUS) : $(OBJS_BONUS) 
	$(CC) $(CFLAGS) $(SAN) $(OBJS_BONUS) $(LIB) -lmlx -framework OpenGL -framework AppKit -o $(NAME_BONUS)

all: $(NAME)
bonus: $(NAME_BONUS)

clean : 
	$(RM) $(OBJS) $(OBJS_BONUS)
	
fclean : clean
	$(RM) $(NAME) $(NAME_BONUS)

re : fclean all
