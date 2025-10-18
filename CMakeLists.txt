cmake_minimum_required(VERSION 4.1)
project(HWP LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 23)

include_directories(.)

add_executable(HWP
        server.cpp
        WordCheck.c
)

add_executable(HWP-client client.cpp)
