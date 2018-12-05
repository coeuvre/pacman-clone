#!/bin/bash

cd `dirname $0`

export DYLD_LIBRARY_PATH=build/:$DYLD_LIBRARY_PATH
./build.sh && build/pacman
