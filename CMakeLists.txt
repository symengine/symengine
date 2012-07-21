cmake_minimum_required(VERSION 2.8)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_SOURCE_DIR}/cmake/UserOverride.cmake)

project(csympy)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Make sure that CMAKE_BUILD_TYPE is either Debug or Release:
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug
        CACHE STRING "Build type (Debug, Release)" FORCE)
endif ()
if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR
        CMAKE_BUILD_TYPE STREQUAL "Release"))
    message("${CMAKE_BUILD_TYPE}")
    message(FATAL_ERROR "CMAKE_BUILD_TYPE must be one of: Debug, Release (current value: '${CMAKE_BUILD_TYPE}')")
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter")
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-parameter")
endif()

enable_testing()
add_subdirectory(src)

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
