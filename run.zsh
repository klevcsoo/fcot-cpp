#!/bin/zsh
set -e
set -o localoptions -o localtraps

trap 'rm ./main' INT

gcc ./main.cpp -o ./main -lstdc++ -std=c++17
./main ${1} ${2}
rm ./main
