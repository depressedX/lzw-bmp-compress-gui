# cmake_minimum_required(VERSION <specify CMake version here>)
project(LZW)

set(CMAKE_CXX_STANDARD 11)

add_executable(LZW main.cpp bitIO.h)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")