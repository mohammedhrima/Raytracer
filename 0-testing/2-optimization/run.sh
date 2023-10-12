# cc main.c -fsanitize=address -g3 -lmlx -framework OpenGL -framework AppKit  -lm -O3

kill $(ps aux | grep "Preview" | awk 'NR==2{print $2}')
c++ main.cpp -c
c++ main.o  -fsanitize=address -g3 -O3 -o a.out
./a.out > image.ppm
open image.ppm
rm -rf a.out main.o

