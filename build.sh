#!/bin/bash

cd `dirname $0`

PROJECT_DIR=`pwd`
CC=clang
CXXFLAGS="-isystem /usr/local/include -Weverything -Wno-old-style-cast -Wno-pedantic"
GAME_SRC=$PROJECT_DIR/src/game.cpp
PLATFORM_SRC=$PROJECT_DIR/src/sdl_platform.cpp

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

$CC $CXXFLAGS $GAME_SRC -dynamiclib -o libgame.dylib
$CC $CXXFLAGS $PLATFORM_SRC -lSDL2 -framework OpenGL -o pacman
