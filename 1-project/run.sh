# brew install libomp

# if [ "$1" = "1" ]; then
#     file="main.c"
# else
#     file="tmp.c"
# fi

file="main.c"
cc $file -O3 -Xpreprocessor -fopenmp ~/goinfre/homebrew/opt/libomp/lib/libomp.dylib -lmlx -framework OpenGL -framework AppKit -mavx -ffast-math #-fsanitize=address -g3
./a.out
rm -rf a.out* *.o
