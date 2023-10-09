kill $(ps aux | grep "Preview" | awk 'NR==2{print $2}')
# rm -rf image.ppm
c++ 005/main.cpp  -fsanitize=address -g3 -O3 && ./a.out > image.ppm
open image.ppm