cmake_minimum_required(VERSION 3.26)
project(Sem)

set(CMAKE_CXX_STANDARD 20)

add_executable(Sem main.cpp
        server/server.cpp
        server/User.cpp
        server/User.h
        server/Database.h
        server/Table/Table.cpp
        server/Table/Table.h
        server/Table/TableColumnBase.h
        server/Table/IntColumn.cpp
        server/Table/DoubleColumn.cpp
        server/Table/StringColumn.cpp
        server/Table/BoolColumn.cpp
        server/Table/DateColumn.cpp
        server/Database.cpp
        server/Table/TableColumnBase.cpp
)

target_link_libraries(Sem ws2_32)
