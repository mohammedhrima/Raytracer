# homebrew
brew="$HOME/goinfre/homebrew"

# file to compile
file="1-project/main.cpp"
if [ $1 ]; then 
    file=$1
fi

if [ ! -e "$brew" ]; then
  echo "brew not found"
  curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
  source ~/.zshrc
fi

# threads
threads="$brew/opt/libomp/lib/libomp.dylib"
if [ ! -e "$threads" ]; then
  echo "libmop not found"
  brew install libomp
fi

if [ ! -e "$brew/lib/libSDL2-2.0.0.dylib" ]; then
  echo "sdl2 not found"
  brew install sdl2
fi

threads="-Xpreprocessor -fopenmp $threads"
sdl="$brew/lib/libSDL2-2.0.0.dylib"

comp=""
if [[ "$file" == *.c ]]; then
  comp="cc"
  dependencies="$threads -lmlx -framework OpenGL -framework AppKit -mavx -ffast-math"
elif [[ "$file" == *.cpp ]]; then
  comp="c++"
  dependencies=$sdl
else
  echo "Unsupported file extension. Make sure your file is .c or .cpp."
  exit 1
fi

# $flags="-Wall -Werror -Wextra"
cmd="$comp $flags $file -O3 $dependencies"
cmd=$cmd #"-fsanitize=thread -g3"

echo $cmd
# compile and run
$cmd
./a.out
rm -rf a.out* *.o
