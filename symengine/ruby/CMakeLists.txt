cmake_minimum_required(VERSION 2.8)
project(ruby_wrapper)

find_package(SymEngine REQUIRED CONFIG
             PATHS ${SYMENGINE_DIR}
             PATH_SUFFIXES lib/CMake/symengine CMake/)
set(CMAKE_BUILD_TYPE ${SYMENGINE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS_RELEASE ${SYMENGINE_CXX_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_DEBUG ${SYMENGINE_CXX_FLAGS_DEBUG})
include_directories(${SYMENGINE_INCLUDE_DIRS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
find_package(Ruby REQUIRED)

message("Copying source of ruby wrappers into: ${CMAKE_CURRENT_BINARY_DIR}")
file(COPY ${ruby_wrapper_SOURCE_DIR}/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(ext/symengine)