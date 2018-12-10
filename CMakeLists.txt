cmake_minimum_required(VERSION 3.13)
project(pacman_clone)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

include_directories(src)

add_library(game SHARED src/game.cpp)

add_executable(pacman src/sdl_platform.cpp)
target_link_libraries(pacman SDL2 ${OPENGL_LIBRARIES})
add_dependencies(pacman game)
