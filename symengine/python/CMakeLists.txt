cmake_minimum_required(VERSION 2.8)
project(python_wrapper)

find_package(SymEngine REQUIRED CONFIG
             PATHS ${SYMENGINE_DIR}
             PATH_SUFFIXES lib/CMake/symengine CMake/)
set(CMAKE_BUILD_TYPE ${SYMENGINE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS_RELEASE ${SYMENGINE_CXX_FLAGS_RELEASE})
set(CMAKE_CXX_FLAGS_DEBUG ${SYMENGINE_CXX_FLAGS_DEBUG})
include_directories(${SYMENGINE_INCLUDE_DIRS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
find_package(Python REQUIRED)
find_package(Cython REQUIRED)

message("Copying source of python wrappers into: ${CMAKE_CURRENT_BINARY_DIR}")
file(COPY symengine/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/symengine)

add_subdirectory(symengine)
