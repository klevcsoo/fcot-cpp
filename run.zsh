#!/bin/zsh
set -e
set -o localoptions -o localtraps

trap 'rm ./main' INT

echo "Building..."
gcc main.cpp -std=c++17 -lstdc++ $(pkg-config --cflags --libs opencv) -o main 
echo "Build succeeded!\n\n"

./main ${1} ${2}
rm ./main
