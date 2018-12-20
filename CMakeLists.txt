cmake_minimum_required(VERSION 3.13)
project(pacman_clone)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

include_directories(SYSTEM /usr/local/include ${PROJECT_SOURCE_DIR}/lib)
link_directories(/usr/local/lib)

include_directories(src)

add_library(game SHARED src/game/game.cpp)

add_executable(pacman src/platform/platform_sdl.cpp)
target_link_libraries(pacman SDL2 ${OPENGL_LIBRARIES})
add_dependencies(pacman game)
