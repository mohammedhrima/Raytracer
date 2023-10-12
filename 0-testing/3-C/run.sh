cc main.c -c
cc main.o -lmlx -framework OpenGL -framework AppKit -O3 -mavx -ffast-math
./a.out
rm -rf a.out *.o