
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

find_package(Boost 1.36.0 COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

list(APPEND CMAKE_CXX_FLAGS -std=c++11)
list(APPEND CMAKE_C_FLAGS -std=c99)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(short_float short_float.cc short_float.h)

add_executable(dumpTables dumpTables.cc)

gengetopt_rule(igbzip.opts igbzip.ggo)
add_executable(igbzip igbzip.cc igbzip.opts.c)
target_link_libraries(igbzip short_float ${Boost_FILESYSTEM_LIBRARY_RELEASE} ${Boost_SYSTEM_LIBRARY_RELEASE} z bz2)

gengetopt_rule(igbunzip.opts igbunzip.ggo)
add_executable(igbunzip igbunzip.cc igbunzip.opts.c)
target_link_libraries(igbunzip short_float ${Boost_FILESYSTEM_LIBRARY_RELEASE} ${Boost_SYSTEM_LIBRARY_RELEASE} bz2)

add_executable(testConversion testConversion.cc)
target_link_libraries(testConversion short_float)