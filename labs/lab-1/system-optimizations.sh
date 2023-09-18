#!/bin/bash

SRC_DIR=binary-tree-sum/simple
EXECUTABLE_NAME=system-optimization.out
GUIDED_EXECUTABLE_NAME=guided-system-optimization.out

compileFlags="-O3 -march=native -funroll-loops"

echo "Options: $compileFlags -fipa-pta -flto=auto"
g++ $compileFlags -fipa-pta -flto=auto -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
time ./$EXECUTABLE_NAME
echo
executableSize=$(du -b "./$EXECUTABLE_NAME")
echo "Executable size: $executableSize bytes"
echo "======================================"
echo "======================================"
rm $EXECUTABLE_NAME

echo "Options: $compileFlags -fprofile-generate/-fprofile-use"
g++ $compileFlags -fprofile-generate -o $GUIDED_EXECUTABLE_NAME $SRC_DIR/*.cpp
./$GUIDED_EXECUTABLE_NAME
g++ $compileFlags -fprofile-use -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
time ./$EXECUTABLE_NAME
echo
executableSize=$(du -b "./$EXECUTABLE_NAME")
echo "Executable size: $executableSize bytes"
echo "======================================"
echo "======================================"
rm $GUIDED_EXECUTABLE_NAME
rm ./*.gcda
rm $EXECUTABLE_NAME

echo "Options: $compileFlags -fipa-pta -flto=auto -fprofile-generate/-fprofile-use"
g++ $compileFlags -fipa-pta -flto=auto -fprofile-generate -o $GUIDED_EXECUTABLE_NAME $SRC_DIR/*.cpp
./$GUIDED_EXECUTABLE_NAME
g++ $compileFlags -fipa-pta -flto=auto -fprofile-use -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
time ./$EXECUTABLE_NAME
echo
executableSize=$(du -b "./$EXECUTABLE_NAME")
echo "Executable size: $executableSize bytes"
echo "======================================"
echo "======================================"
rm $GUIDED_EXECUTABLE_NAME
rm ./*.gcda
rm $EXECUTABLE_NAME