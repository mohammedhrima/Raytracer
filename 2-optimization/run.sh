# cc main.c -fsanitize=address -g3 -lmlx -framework OpenGL -framework AppKit  -lm -O3

kill $(ps aux | grep "Preview" | awk 'NR==2{print $2}')
c++ main.cpp  -fsanitize=address -g3 -O3 
./a.out > image.ppm
open image.ppm
rm -rf a.out main.o

