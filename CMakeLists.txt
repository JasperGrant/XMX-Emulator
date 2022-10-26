cmake_minimum_required(VERSION 3.22)
project(Assignment_1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Assignment_1 main.h main.c decoder.c executor.c memoryandPSW.c loader.c)
