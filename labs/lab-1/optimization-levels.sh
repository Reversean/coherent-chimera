#!/bin/bash

SRC_DIR=binary-tree-sum/simple
EXECUTABLE_NAME=optimization-levels.out

options=(
    "-O0" "-Os" "-O1" "-O2" "-O3" "-O2 -march=native" "-O3 -march=native" \
    "-O2 -march=native -funroll-loops" "-O3 -march=native -funroll-loops"
)

for i in ${!options[*]}
do
    echo "Options: ${options[$i]}"
    g++ ${options[$i]} -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
    time ./$EXECUTABLE_NAME
    executableSize=$(du -b "./$EXECUTABLE_NAME")
    echo "Executable size: $executableSize bytes"
    echo "======================================"
    echo "======================================"
    rm $EXECUTABLE_NAME
done
