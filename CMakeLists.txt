
project(compress_igb)
cmake_minimum_required(VERSION 2.6)

find_program(GENGETOPT gengetopt)
mark_as_advanced(GENGETOPT)
function(gengetopt_rule target source)
  add_custom_command(
  OUTPUT ${target}.c
  MAIN_DEPENDENCY ${source}
  COMMAND ${GENGETOPT} -i${CMAKE_CURRENT_SOURCE_DIR}/${source} -l -F ${target}
  VERBATIM
  )
endfunction(gengetopt_rule)

add_definitions(-std=c99)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(short_float short_float.c short_float.h)

add_executable(dumpTables dumpTables.c)

gengetopt_rule(igbzip.opts igbzip.ggo)
add_executable(igbzip igbzip.c igbzip.opts.c)
target_link_libraries(igbzip short_float z)

gengetopt_rule(igbunzip.opts igbunzip.ggo)
add_executable(igbunzip igbunzip.c igbunzip.opts.c)
target_link_libraries(igbunzip short_float z)
