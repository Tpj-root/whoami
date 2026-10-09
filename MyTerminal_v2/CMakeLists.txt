cmake_minimum_required(VERSION 3.16)

project(MyTerminal
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(myterm
    src/main.cpp
    src/Terminal.cpp
)

target_include_directories(myterm PRIVATE
    src
)