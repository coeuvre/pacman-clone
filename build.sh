#!/bin/bash

cd `dirname $0`

PROJECT_DIR=`pwd`
CC=clang
CXXFLAGS=-Wall
SRC=$PROJECT_DIR/src/sdl_pacman.cpp

if [ ! -d "build" ]; then
    mkdir build
fi

cd build


$CC $CXXFLAGS $SRC -o pacman
