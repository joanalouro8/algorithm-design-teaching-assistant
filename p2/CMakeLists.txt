cmake_minimum_required(VERSION 3.26)

project(myProg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SHARED_SOURCES
    src/parser.cpp
    src/algorithm.cpp
    src/web.cpp
    src/interference.cpp
)

add_executable(myProg
    src/main.cpp
    ${SHARED_SOURCES}
)

add_executable(myCorrection
    correction/main.cpp
    correction/validator.cpp
    ${SHARED_SOURCES}
)

target_include_directories(myCorrection PRIVATE src)