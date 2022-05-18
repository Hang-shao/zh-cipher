cmake_minimum_required(VERSION 3.21)
project(SM3)

set(CMAKE_CXX_STANDARD 14)

add_executable(SM3 main.cpp sm3/sm3.c)
