cmake_minimum_required(VERSION 2.8)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_SOURCE_DIR}/cmake/UserOverride.cmake)

project(csympy)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Make sure that CMAKE_BUILD_TYPE is either Debug or Release:
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release
        CACHE STRING "Build type (Debug, Release)" FORCE)
endif ()
if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
        CMAKE_BUILD_TYPE STREQUAL "Release"))
    message("${CMAKE_BUILD_TYPE}")
    message(FATAL_ERROR "CMAKE_BUILD_TYPE must be one of: Debug, Release (current value: '${CMAKE_BUILD_TYPE}')")
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -ggdb")
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-parameter")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -ggdb")
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-parameter")
endif()

# GMP
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GMP_LIBRARIES})

# Python
set(WITH_PYTHON no
    CACHE BOOL "Build with Python wrappers")

# BFD
set(WITH_BFD yes
    CACHE BOOL "Build with BFD (binutils-dev) support")

if (WITH_BFD)
    find_package(BFD REQUIRED)
    include_directories(${BFD_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${BFD_LIBRARIES})
    set(HAVE_TEUCHOS_BFD yes)
endif()

# CSYMPY_ASSERT
set(WITH_CSYMPY_ASSERT no
    CACHE BOOL "Enable CSYMPY_ASSERT macro")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(HAVE_TEUCHOS_DEBUG yes)
    set(HAVE_TEUCHOS_DEBUG_RCP_NODE_TRACING yes)
endif()

enable_testing()
add_subdirectory(src)
add_subdirectory(benchmarks)

if (WITH_PYTHON)
    # We need to execute the find_package() commands here, so that the
    # variables like ${PYTHON_INSTALL_PATH} are defined here and in all cmake
    # files (as opposed to just subset of cmake files).
    find_package(Python REQUIRED)
    find_package(Cython REQUIRED)
    add_subdirectory(csympy)
endif ()


message("\n")
message("Configuration results")
message("---------------------")
message("C++ compiler: ${CMAKE_CXX_COMPILER}")
message("Build type: ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("C++ compiler flags: ${CMAKE_CXX_FLAGS_DEBUG}")
else ()
    message("C++ compiler flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif ()
message("Installation prefix: ${CMAKE_INSTALL_PREFIX}")

message("WITH_CSYMPY_ASSERT: ${WITH_CSYMPY_ASSERT}")

message("GMP_INCLUDE_DIRS: ${GMP_INCLUDE_DIRS}")
message("GMP_LIBRARIES: ${GMP_LIBRARIES}")

message("WITH_BFD: ${WITH_BFD}")
if (WITH_BFD)
    message("BFD_INCLUDE_DIRS: ${BFD_INCLUDE_DIRS}")
    message("BFD_LIBRARIES: ${BFD_LIBRARIES}")
endif()

message("WITH_PYTHON: ${WITH_PYTHON}")
if (WITH_PYTHON)
    message("Python library: ${PYTHON_LIBRARY}")
    message("Python include path: ${PYTHON_INCLUDE_PATH}")
    message("Python install path: ${PYTHON_INSTALL_PATH}")

    message("Cython path: ${CYTHON_BIN}")
endif ()
