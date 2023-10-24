# homebrew
brew="$HOME/goinfre/homebrew/opt"

# file to compile
file="1-project/main.cpp"
if [ $1 ]; then 
    file=$1
fi

# threads
threads="$brew/libomp/lib/libomp.dylib"
if [ ! -e "$threads" ]; then
  brew install libomp
fi
threads="-Xpreprocessor -fopenmp $threads"

comp=""
if [[ "$file" == *.c ]]; then
  comp="cc"
  dependencies="$threads -lmlx -framework OpenGL -framework AppKit -mavx -ffast-math"
elif [[ "$file" == *.cpp ]]; then
  comp="c++"
  dependencies="$brew/glfw/lib/libglfw.3.3.dylib $brew/glew/lib/libGLEW.dylib"
  dependencies="$dependencies -framework OpenGL"
else
  echo "Unsupported file extension. Make sure your file is .c or .cpp."
  exit 1
fi

echo $file

# compile and run
$comp $file -O3 $dependencies -fsanitize=address -g3
./a.out
rm -rf a.out* *.o
