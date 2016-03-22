# This overrides the default CMake Debug and Release compiler options.
# The user can still specify different options by setting the
# CMAKE_Fortran_FLAGS_[RELEASE,DEBUG] variables (on the command line or in the
# CMakeList.txt). This files serves as better CMake defaults and should only be
# modified if the default values are to be changed. Project specific compiler
# flags should be set in the CMakeList.txt by setting the CMAKE_Fortran_FLAGS_*
# variables.
if (CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
    # gfortran
    set(common "-std=f2008 -Wall -Wextra -Wimplicit-interface -fPIC -Werror -fmax-errors=1")
    set(CMAKE_Fortran_FLAGS_RELEASE_INIT "${common} -O3 -march=native -ffast-math -funroll-loops")
    set(CMAKE_Fortran_FLAGS_DEBUG_INIT   "${common} -g -fcheck=all -fbacktrace")
elseif (CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
    # ifort
    set(common "-std2008 -warn all")
    set(CMAKE_Fortran_FLAGS_RELEASE_INIT "${common} -xHOST -O3 -no-prec-div -static")
    set(CMAKE_Fortran_FLAGS_DEBUG_INIT   "${common} -check all")
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # g++
    set(common "-std=c++11 -Wall -Wextra -fno-common")
    if (NOT MINGW)
        set(common "${common} -fPIC")
    else()
        set(common "${common} -D_hypot=hypot -Doff64_t=_off64_t")
        if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
            set(common "${common} -DMS_WIN64")
        endif()
    endif()
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "${common} -O3 -march=native -ffast-math -funroll-loops")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT   "${common} -g")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # icpc
    set(common "-std=c++11 -Wall -fno-common")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "${common} -xHOST -O3 -no-prec-div")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT   "${common} -g -O0")
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    # clang
    set(common "-std=c++11 -Wall -Wextra -fPIC")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "${common} -O3 -march=native -ffast-math -funroll-loops")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT   "${common} -g")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "PGI")
    # pgcpp
    set(common "--gnu --c++11 -D__GXX_EXPERIMENTAL_CXX0X__")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "${common}")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT   "${common}")
endif ()
