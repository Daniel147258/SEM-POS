cmake_minimum_required(VERSION 3.26)
project(Sem)

set(CMAKE_CXX_STANDARD 17)

add_executable(Sem main.cpp
        Table.cpp
        Table.h)
