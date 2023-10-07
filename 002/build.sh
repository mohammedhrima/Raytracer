var=false
flags="-fsanitize=address -g3 -lm -O3"

if $var
then
    c++ main.cpp $flags && ./a.out > image.ppm
else
    cc main.c $flags && ./a.out > image.ppm
fi