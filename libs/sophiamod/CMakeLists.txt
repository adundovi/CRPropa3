cmake_minimum_required(VERSION 2.6)

enable_language(Fortran)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(sophiamod STATIC
    sophia_interface.f
)

set_target_properties(sophiamod PROPERTIES COMPILE_FLAGS -fPIC)
