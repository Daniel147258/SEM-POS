cmake_minimum_required(VERSION 3.26)
project(Sem)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sem server/main.cpp
        server/Table/Table.cpp
)
