cmake_minimum_required(VERSION 3.30)
project(libc C)

set(CMAKE_C_STANDARD 11)

add_library(libc SHARED libc.c)

set_target_properties(libc PROPERTIES OUTPUT_NAME "c")