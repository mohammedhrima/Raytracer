#!/bin/bash

brew="$HOME/goinfre/homebrew"
file="3.project/main.cpp"

if [ "$1" ]; then
    file="$1"
fi

fun () {
    trap stop_loading_animation SIGINT
    case $1 in
    (-clean | -c) 42-wizzard-clean 2> /dev/null ;;
(-storage | -s) 42-wizzard-storage ;;
(-reset | -r) 42-wizzard-reset ;;
(-update | -u) sh ~/.42-wizzard-updater.sh > /dev/null 2>&1 ;;
(-brew) 42-wizzard-brew ;;
(-docker) 42-wizzard-docker 2> /dev/null ;;
(-code) 42-wizzard-code ;;
(-ssh) 42-wizzard-ssh ;;
(-nvm) 42-wizzard-nvm 2> /dev/null ;;
(-pip) 42-wizzard-pip 2> /dev/null ;;
(-node) 42-wizzard-node 2> /dev/null ;;
(-oh-my-zsh | -omz) 42-wizzard-oh-my-zsh 2> /dev/null ;;
(-ds-store | -ds) 42-wizzard-ds-store ;;
(-help | -h) 42-wizzard-help ;;
(*) echo 42: "Unknown flag: $1"
echo "Usage: 42 -flag"
42-wizzard-help ;;
esac
}

if [ "$(uname)" == "Darwin" ]; then
    if [ ! -e "$HOME/.42-wizzard.sh" ]; then
        echo "42-wizzard is not found. Installing 42-wizzard..."
        curl -fsSL https://raw.githubusercontent.com/0xShady/42_wizzard/main/assistance/42-wizzard-installer.sh | zsh && source ~/.zshrc
    fi
    
    if ! command -v brew &>/dev/null; then
        echo "Homebrew (brew) is not found. Installing Homebrew..."
        fun -brew
        source ~/.zshrc
    fi
    
    sdl="$brew/lib/libSDL2-2.0.0.dylib"
    if [ ! -e "$sdl" ]; then
        echo "sdl2 not found. Installing SDL2..."
        brew install sdl2
        source ~/.zshrc
    fi
    
    comp=""
    dependencies=""
    if [[ "$file" == *.c ]]; then
        comp="cc"
        dependencies="$threads -lmlx -framework OpenGL -framework AppKit -mavx -ffast-math"
        
        threads="$brew/opt/libomp/lib/libomp.dylib"
        if [ ! -e "$threads" ]; then
            echo "libomp not found. Installing libomp..."
            brew install libomp
            source ~/.zshrc
        fi
        elif [[ "$file" == *.cpp ]]; then
        comp="c++"
        dependencies="$sdl"
    else
        echo "Unsupported file extension. Make sure your file is .c or .cpp."
        exit 1
    fi
    
    elif [ "$(uname)" == "Linux" ]; then
    comp="clang++ -lSDL2 -fsanitize=address -g3"
    if [ ! -f /usr/include/SDL2/SDL.h ] && [ ! -f /usr/local/include/SDL2/SDL.h ]; then
        echo "SDL2 is not installed"
        git clone https://github.com/libsdl-org/SDL.git -b SDL2
        cd SDL
        mkdir build
        cd build
        ../configure
        make
        cd ../../
        rm -rf SDL
    fi
fi

# flags="-fsanitize=address -g3"

cmd="$comp $flags $file -O3 $dependencies"
echo "$cmd"
$cmd
./a.out

rm -rf ./a.out* */*.o
