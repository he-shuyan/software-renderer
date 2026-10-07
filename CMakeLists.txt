cmake_minimum_required(VERSION 3.16)
project(renderer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RENDERER_SOURCES
    src/image.cpp
    src/line.cpp
    src/Mat3.cpp
    src/triangle.cpp
)


add_executable(app
    src/main.cpp
    ${RENDERER_SOURCES}
)

add_executable(tests
    src/tests.cpp
    ${RENDERER_SOURCES}
)