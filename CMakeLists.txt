cmake_minimum_required(VERSION 2.8)

project(csympy)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++0x")
endif()

enable_testing()
add_subdirectory(src)
