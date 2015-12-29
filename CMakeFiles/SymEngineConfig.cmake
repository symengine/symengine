# To use SymEngine from another CMake project include the following in your
# `CMakeLists.txt` file

#    `find_package(SymEngine CONFIG)`

# You can give the path to the SymEngine installation directory if it was
# installed to a non standard location by,

#    `find_package(SymEngine CONFIG Paths /path/to/install/dir)`

# Alternatively, you can give the path to the build directory.

# Variable exported are
# SYMENGINE_BUILD_TYPE         - Cofiguration Type Debug or Release
# SYMENGINE_INCLUDE_DIRS       - Header file directories
# SYMENGINE_LIBRARIES          - SymEngine libraries and dependency libraries to link against
# SYMENGINE_FOUND              - Set to yes
# SYMENGINE_CXX_FLAGS_RELEASE  - C++ flags for Release configuration
# SYMENGINE_CXX_FLAGS_DEBUG    - C++ flags for Debug configuration
# SYMENGINE_C_FLAGS_RELEASE    - C flags for Release configuration
# SYMENGINE_C_FLAGS_DEBUG      - C flags for Debug configuration

# An example project would be,
#
# cmake_minimum_required(VERSION 2.8)
# find_package(symengine CONFIG)
# set(CMAKE_CXX_FLAGS_RELEASE ${SYMENGINE_CXX_FLAGS_RELEASE})
#
# include_directories(${SYMENGINE_INCLUDE_DIRS})
# add_executable(example main.cpp)
# target_link_libraries(example ${SYMENGINE_LIBRARIES})
#

cmake_minimum_required(VERSION 2.8)

set(SYMENGINE_CXX_FLAGS_RELEASE "-std=c++0x -Wall -Wextra -fno-common -fPIC -O3 -march=native -ffast-math -funroll-loops -Wno-unused-parameter")
set(SYMENGINE_CXX_FLAGS_DEBUG "-std=c++0x -Wall -Wextra -fno-common -fPIC -g -Wno-unused-parameter -ggdb")
set(SYMENGINE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(SYMENGINE_C_FLAGS_DEBUG "-g")

# ... for the build tree
get_filename_component(SYMENGINE_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(SYMENGINE_BUILD_TREE no)

if(SYMENGINE_BUILD_TREE)
    set(CMAKE_MODULE_PATH "${SYMENGINE_CMAKE_DIR}/cmake/")
    set(SYMENGINE_INSTALL_CMAKE_DIR "${SYMENGINE_CMAKE_DIR}")
    set(SYMENGINE_INCLUDE_DIRS /home/akash/symengine;/home/akash/symengine;/home/akash/symengine/symengine/teuchos;/home/akash/symengine/symengine/teuchos ${SYMENGINE_CMAKE_DIR} ${SYMENGINE_CMAKE_DIR}/symengine/teuchos)
else()
    set(CMAKE_MODULE_PATH ${SYMENGINE_CMAKE_DIR})
    set(SYMENGINE_INSTALL_CMAKE_DIR "/usr/local/lib/cmake/symengine")
    set(SYMENGINE_INCLUDE_DIRS "${SYMENGINE_CMAKE_DIR}/../../../include")
endif()

if(NOT TARGET symengine)
    include("${SYMENGINE_CMAKE_DIR}/SymEngineTargets.cmake")
endif()


set(SYMENGINE_GMP_LIBRARIES /usr/lib/x86_64-linux-gnu/libgmpxx.so;/usr/lib/x86_64-linux-gnu/libgmp.so)
set(SYMENGINE_GMP_INCLUDE_DIRS /usr/include)

if (TARGET gmp)
    # Avoid defining targets again
    set(SYMENGINE_SKIP_DEPENDENCIES yes CACHE BOOL "Skip finding dependencies")
else()
    set(SYMENGINE_SKIP_DEPENDENCIES no CACHE BOOL "Skip finding dependencies")
endif()

# For each package give hints with paths that symengine found when it was built
# Priority is given for absolute path and then relative path
foreach(PKG GMP)
    if (NOT SYMENGINE_SKIP_DEPENDENCIES)
        foreach(LIB ${SYMENGINE_${PKG}_LIBRARIES})
            get_filename_component(TEMP ${LIB} PATH)
            # Add Absolute Path to the beginning
            set(${PKG}_LIBRARIES ${TEMP} ${${PKG}_LIBRARIES})
            file(RELATIVE_PATH TEMP ${SYMENGINE_INSTALL_CMAKE_DIR} ${TEMP})
            # Add Relative Path to the end
            set(${PKG}_LIBRARIES ${${PKG}_LIBRARIES} ${SYMENGINE_CMAKE_DIR}/${TEMP})
        endforeach()
        foreach(DIR ${SYMENGINE_${PKG}_INCLUDE_DIRS})
            set(${PKG}_INCLUDE_DIRS ${DIR} ${${PKG}_INCLUDE_DIRS})
            file(RELATIVE_PATH TEMP ${SYMENGINE_INSTALL_CMAKE_DIR} ${DIR})
            set(${PKG}_INCLUDE_DIRS ${${PKG}_INCLUDE_DIRS} ${SYMENGINE_CMAKE_DIR}/${TEMP})
        endforeach()
        # Find the package using SymEngine's FindPKG modules
        find_package(${PKG} REQUIRED)
    endif()

    set(SYMENGINE_INCLUDE_DIRS ${SYMENGINE_INCLUDE_DIRS} ${${PKG}_INCLUDE_DIRS})
    set(SYMENGINE_LIBRARIES ${SYMENGINE_LIBRARIES} ${${PKG}_TARGETS})
endforeach()

#Use CMake provided find_package(BOOST) module
if (NOT "" STREQUAL "")
    set(SYMENGINE_INCLUDE_DIRS ${SYMENGINE_INCLUDE_DIRS} )
    set(SYMENGINE_LIBRARIES ${SYMENGINE_LIBRARIES} )
endif()

list(REMOVE_DUPLICATES SYMENGINE_INCLUDE_DIRS)

foreach(LIB symengine teuchos)
    # Remove linking of dependencies to later add them as targets
    set_target_properties(${LIB} PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "")
    set_target_properties(${LIB} PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG "")
endforeach()

set(SYMENGINE_LIBRARIES symengine teuchos ${SYMENGINE_LIBRARIES})
set(SYMENGINE_BUILD_TYPE "Release")
set(SYMENGINE_FOUND yes)
