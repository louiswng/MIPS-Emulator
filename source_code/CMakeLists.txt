cmake_minimum_required(VERSION 2.5)
project(JZMid)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(JZMid
        AS2MC.cpp
        main.cpp
        MIPSVM.cpp)
