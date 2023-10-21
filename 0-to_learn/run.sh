# brew install libomp
cc file.c -Xpreprocessor -fopenmp ~/goinfre/homebrew/opt/libomp/lib/libomp.dylib -lmlx -framework OpenGL -framework AppKit -O3 -mavx -ffast-math #-fsanitize=address -g3
./a.out
rm -rf a.out* *.o
