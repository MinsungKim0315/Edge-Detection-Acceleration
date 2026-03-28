cmake_minimum_required(VERSION 3.16)
project(edge_detector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(edge_detector
    src/main.cpp
    src/image_io.cpp
    src/gaussian.cpp
    src/sobel.cpp
    src/nms.cpp
    src/threshold.cpp
    src/hysteresis.cpp
)

target_include_directories(edge_detector PRIVATE include)
