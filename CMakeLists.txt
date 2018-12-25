cmake_minimum_required(VERSION 3.13)
project(pacman_clone)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(src)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/lib/stb)
include_directories(SYSTEM ${SDL2_INCLUDE_DIRS})

add_library(game SHARED src/game/game.cpp)

add_executable(pacman src/platform/platform_sdl.cpp)
add_dependencies(pacman game)
target_link_libraries(pacman ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
