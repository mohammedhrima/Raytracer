# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/29 08:49:10 by mhrima            #+#    #+#              #
#    Updated: 2023/11/01 17:42:02 by mhrima           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -rf

SRCS = main.c #mlx.c object_hit.c objects.c pixels.c traceray.c utils.c vectors0.c vectors1.c
OBJS = $(SRCS:.c=.o)

NAME = ./minirt
NAME_BONUS = ./minirt_bonus

SAN = -fsanitize=address -g3

%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME) : $(OBJS) 
	$(CC) $(CFLAGS) $(SAN) $(OBJS) $(LIB) -lm -lmlx -lXext -lX11 -o $(NAME)

$(NAME_BONUS) : $(OBJS_BONUS) 
	$(CC) $(CFLAGS) $(SAN) $(OBJS_BONUS) $(LIB) -lm -lmlx -lXext -lX11 -o $(NAME_BONUS)

all: $(NAME)
bonus: $(NAME_BONUS)

clean : 
	$(RM) $(OBJS) $(OBJS_BONUS)
	
fclean : clean
	$(RM) $(NAME) $(NAME_BONUS)

re : fclean all
