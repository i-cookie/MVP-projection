cmake_minimum_required(VERSION 3.16.3)
project(Assignment2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(Assignment2 PUBLIC usr/include)

add_executable(Ass2 main.cpp rasterizer.cpp Triangle.cpp)
target_link_libraries(Ass2 ${OpenCV_LIBS})