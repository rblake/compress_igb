
project(compress_igb)

add_definitions(-std=c99)

add_library(short_float short_float.c short_float.h)

add_executable(dumpTables dumpTables.c)

