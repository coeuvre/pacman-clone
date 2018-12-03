#!/bin/bash

cd `dirname $0`

PROJECT_DIR=`pwd`
CC=clang
CXXFLAGS="-isystem /usr/local/include -Weverything"
SRC=$PROJECT_DIR/src/sdl_main.cpp

if [ ! -d "build" ]; then
    mkdir build
fi

cd build


$CC $CXXFLAGS $SRC -lSDL2 -framework OpenGL -o pacman
