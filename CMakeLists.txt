cmake_minimum_required(VERSION 3.13)
project(pacman_clone)

set(CMAKE_CXX_STANDARD 14)

find_package(OPENGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(FREETYPE REQUIRED)

include_directories(src)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/lib/stb)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/lib/glad/include)
include_directories(SYSTEM ${SDL2_INCLUDE_DIRS})
include_directories(SYSTEM ${FREETYPE_INCLUDE_DIRS})

add_executable(pacman src/pacman.cpp ${PROJECT_SOURCE_DIR}/lib/glad/src/glad.c)
target_link_libraries(pacman ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES})
