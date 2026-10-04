cmake_minimum_required(VERSION 3.16)
project(renderer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    src/main.cpp
    src/image.cpp
    src/line.cpp
    src/Mat3.cpp
)
