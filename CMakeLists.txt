
project(compress_igb)
cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c99)

add_library(short_float short_float.c short_float.h)

add_executable(dumpTables dumpTables.c)

add_executable(igbzip igbzip.c)
target_link_libraries(igbzip short_float z)