cmake_minimum_required(VERSION 3.15)
project(FS)

set(CMAKE_CXX_STANDARD 14)

add_executable(FS fs.c fs.h disk.h disk.c)