rm -rf a.out
cc main.c -o a.out -L. -l:Libft/libft.a -lm -lmlx -lXext -lX11 -fsanitize=address -g3
./a.out $1