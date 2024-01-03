cmake_minimum_required(VERSION 3.26)
project(Sem)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sem main.cpp
        server/server.cpp
        server/Database.cpp
        server/Table/Table.cpp
)

target_link_libraries(Sem ws2_32)