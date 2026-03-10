cmake_minimum_required(VERSION 3.26)

project(myProg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(myProg
    src/main.cpp
    src/parser.cpp
)
