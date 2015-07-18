cmake_minimum_required(VERSION 2.8)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_SOURCE_DIR}/cmake/UserOverride.cmake)

project(symengine)

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

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -ggdb")
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-parameter")
endif()

# Check c++11 support
if (NOT(MSVC))
    try_compile(CXX11 "${CMAKE_CURRENT_BINARY_DIR}/cxx" "${CMAKE_SOURCE_DIR}/cmake/checkcxx11.cpp"
        COMPILE_DEFINITIONS "${CMAKE_CXX_FLAGS_RELEASE}" OUTPUT_VARIABLE CXX11_ERROR_LOG)
    if (NOT ${CXX11})
        message(FATAL_ERROR "Compiler does not support C++11 constructs. \n"
            "CXX11 Error Log : \n${CXX11_ERROR_LOG}")
    endif()
endif()

if ((CMAKE_CXX_COMPILER_ID MATCHES Clang) AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    try_compile(CHECK_CLANG "${CMAKE_CURRENT_BINARY_DIR}/cxx" "${CMAKE_SOURCE_DIR}/cmake/checkclang.cpp")
    if (NOT ${CHECK_CLANG})
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D__extern_always_inline=inline" )
    endif()
endif()

# GMP
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GMP_LIBRARIES})

# Check gmpxx
if (NOT(MSVC))
    try_compile(GMPXX "${CMAKE_CURRENT_BINARY_DIR}/cxx" "${CMAKE_SOURCE_DIR}/cmake/checkgmpxx.cpp"
        CMAKE_FLAGS "-DCOMPILE_DEFINITIONS=${CMAKE_CXX_FLAGS_RELEASE}"
        CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${GMP_INCLUDE_DIRS}"
        CMAKE_FLAGS "-DLINK_LIBRARIES=${GMP_LIBRARIES}"
        OUTPUT_VARIABLE GMPXX_ERROR_LOG)
    if (NOT ${GMPXX})
        message(FATAL_ERROR "GMP library being linked is not supported by CMAKE_CXX_COMPILER used. \n"
            "Recompile GMP with C++ support using ${CMAKE_CXX_COMPILER}, remove CMakeCache.txt and try again.\n"
            "Error Log : \n${GMPXX_ERROR_LOG}")
    endif()
endif()

# ECM
set(WITH_ECM no
    CACHE BOOL "Build with ECM (libecm-dev) support")

if (WITH_ECM)
    find_package(ECM REQUIRED)
    include_directories(${ECM_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${ECM_LIBRARIES})
    set(HAVE_SYMENGINE_ECM yes)
endif()

# PRIMESIEVE
set(WITH_PRIMESIEVE no
    CACHE BOOL "Build with primesieve")

if (WITH_PRIMESIEVE)
    find_package(PRIMESIEVE REQUIRED)
    include_directories(${PRIMESIEVE_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${PRIMESIEVE_LIBRARIES})
    set(HAVE_SYMENGINE_PRIMESIEVE yes)
endif()

# ARB
set(WITH_ARB no
    CACHE BOOL "Build with Arb")
set(HAVE_SYMENGINE_ARB False)

# MPFR
set(WITH_MPFR no
    CACHE BOOL "Build with MPFR")
set(HAVE_SYMENGINE_MPFR False)

# MPC
set(WITH_MPC no
    CACHE BOOL "Build with MPC")
set(HAVE_SYMENGINE_MPC False)

if (WITH_ARB)
    find_package(FLINT REQUIRED)
    include_directories(${FLINT_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${FLINT_LIBRARIES})

    # Workaround for https://github.com/fredrik-johansson/arb/issues/24
    include_directories(${FLINT_INCLUDE_DIRS}/flint)

    set(WITH_MPFR yes)

    find_package(ARB REQUIRED)
    include_directories(${ARB_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${ARB_LIBRARIES})
    set(HAVE_SYMENGINE_ARB True)
endif()

if (WITH_MPC)
    find_package(MPC REQUIRED)
    include_directories(${MPC_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${MPC_LIBRARIES})
    set(HAVE_SYMENGINE_MPC True)
    set(WITH_MPFR yes)
endif()

# Piranha
set(WITH_PIRANHA no
    CACHE BOOL "Build with Piranha")

# Boost
set(WITH_BOOST no
    CACHE BOOL "Build with Boost")

# PTHREAD
set(WITH_PTHREAD no
    CACHE BOOL "Build with PTHREAD")

if (WITH_PIRANHA)
    set(WITH_BOOST yes)
    set(WITH_PTHREAD yes)
    set(WITH_MPFR yes)
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

    find_package(Piranha REQUIRED)
    include_directories(${PIRANHA_INCLUDE_DIRS})
    set(HAVE_SYMENGINE_PIRANHA yes)
endif()

if (WITH_PTHREAD)
    find_package(PTHREAD REQUIRED)
    include_directories(${PTHREAD_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${PTHREAD_LIBRARIES})
    set(HAVE_SYMENGINE_PTHREAD yes)
endif()

if (WITH_BOOST)
    find_package(Boost REQUIRED COMPONENTS date_time filesystem system)
    include_directories(${Boost_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${Boost_LIBRARIES})
    set(HAVE_SYMENGINE_BOOST yes)
endif()

if (WITH_MPFR)
    find_package(MPFR REQUIRED)
    include_directories(${MPFR_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${MPFR_LIBRARIES})
    set(HAVE_SYMENGINE_MPFR True)
endif()

# Python
set(WITH_PYTHON no
    CACHE BOOL "Build with Python wrappers")

#Ruby
set(WITH_RUBY no
    CACHE BOOL "Build with Ruby bindings")

# BFD
set(WITH_BFD no
    CACHE BOOL "Build with BFD (binutils-dev) support")

if (WITH_BFD)
    find_package(BFD REQUIRED)
    find_package(LINKH REQUIRED)
    include_directories(${BFD_INCLUDE_DIRS} ${LINKH_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${BFD_LIBRARIES})
    set(HAVE_TEUCHOS_BFD yes)
    set(HAVE_TEUCHOS_LINK yes)

    set(HAVE_TEUCHOS_EXECINFO yes)
    find_package(EXECINFO REQUIRED)
    include_directories(${EXECINFO_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${EXECINFO_LIBRARIES})
endif()

# TCMalloc
set(WITH_TCMALLOC no
    CACHE BOOL "Build with TCMalloc linked")

if (WITH_TCMALLOC)
    find_package(TCMALLOC REQUIRED)
    set(LIBS ${LIBS} ${TCMALLOC_LIBRARIES})

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
        set(CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
    endif()

endif()

# Doxygen
set(BUILD_DOXYGEN no
    CACHE BOOL "Create C++ API Doxgyen documentation.")

if (BUILD_DOXYGEN)
    find_package(Doxygen REQUIRED)
    if(DOXYGEN_FOUND)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile
            ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
        add_custom_target(doc ALL
            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen" VERBATIM)
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html/ DESTINATION doc)
    endif(DOXYGEN_FOUND)
endif()

# SYMENGINE_ASSERT
set(WITH_SYMENGINE_ASSERT no
    CACHE BOOL "Enable SYMENGINE_ASSERT macro")

# SYMENGINE_RCP
set(WITH_SYMENGINE_RCP yes
    CACHE BOOL "Enable SYMENGINE_RCP support")

# SYMENGINE_THREAD_SAFE
set(WITH_SYMENGINE_THREAD_SAFE no
    CACHE BOOL "Enable SYMENGINE_THREAD_SAFE support")

# TESTS
set(BUILD_TESTS yes
    CACHE BOOL "Build SymEngine tests")

# BENCHMARKS
set(BUILD_BENCHMARKS yes
    CACHE BOOL "Build SymEngine benchmarks")

# OpenMp
set(WITH_OPENMP no
    CACHE BOOL "Build with OpenMP")

if (WITH_OPENMP)
    find_package(OpenMP REQUIRED)
    if(OPENMP_FOUND)
        set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}")
        set(WITH_SYMENGINE_THREAD_SAFE yes)
    endif()
endif()


if (WITH_RUBY)
    find_package(Ruby REQUIRED)
    include_directories(${RUBY_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${RUBY_LIBRARY})
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # In Debug mode we use Teuchos::RCP and enable debugging checks that make
    # the usage 100% safe, as long as the Teuchos guidelines are followed.
    set(WITH_SYMENGINE_RCP no) # Use the Teuchos::RCP
    set(HAVE_TEUCHOS_DEBUG yes) # Enable safety checks
    set(HAVE_TEUCHOS_DEBUG_RCP_NODE_TRACING yes) # Enable safety checks

    set(WITH_SYMENGINE_ASSERT yes) # Also enable assertions
endif()

enable_testing()
add_subdirectory(symengine)
if (BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if (WITH_PYTHON)
    # We need to execute the find_package() commands here, so that the
    # variables like ${PYTHON_INSTALL_PATH} are defined here and in all cmake
    # files (as opposed to just subset of cmake files).
    find_package(Python REQUIRED)
    find_package(Cython REQUIRED)
    add_subdirectory(symengine/python)
endif ()

if (WITH_PYTHON)
    message("Copying source of python wrappers into: ${CMAKE_CURRENT_BINARY_DIR}/symengine/python/symengine")
    file(COPY symengine/python/symengine/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/symengine/python/symengine)
endif()


# At the end we print a nice summary

message("--------------------------------------------------------------------------------")
message("")
message("+-------------------------------+")
message("| Configuration results SUMMARY |")
message("+-------------------------------+")
message("")
message("CMAKE_C_COMPILER:   ${CMAKE_C_COMPILER}")
message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("CMAKE_CONFIGURATION_TYPES: ${CMAKE_CONFIGURATION_TYPES}")
message("CMAKE_CXX_FLAGS_DEBUG:   ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

message("WITH_SYMENGINE_ASSERT: ${WITH_SYMENGINE_ASSERT}")
message("WITH_SYMENGINE_RCP: ${WITH_SYMENGINE_RCP}")
if (NOT WITH_SYMENGINE_RCP)
    message("HAVE_TEUCHOS_DEBUG: ${HAVE_TEUCHOS_DEBUG}")
    message("HAVE_TEUCHOS_DEBUG_RCP_NODE_TRACING: ${HAVE_TEUCHOS_DEBUG_RCP_NODE_TRACING}")
endif ()
message("WITH_SYMENGINE_THREAD_SAFE: ${WITH_SYMENGINE_THREAD_SAFE}")
message("BUILD_TESTS: ${BUILD_TESTS}")
message("BUILD_BENCHMARKS: ${BUILD_BENCHMARKS}")

message("GMP_INCLUDE_DIRS: ${GMP_INCLUDE_DIRS}")
message("GMP_LIBRARIES: ${GMP_LIBRARIES}")

message("WITH_BFD: ${WITH_BFD}")
if (WITH_BFD)
    message("BFD_INCLUDE_DIRS: ${BFD_INCLUDE_DIRS}")
    message("BFD_LIBRARIES: ${BFD_LIBRARIES}")
    message("LINKH_INCLUDE_DIRS: ${LINKH_INCLUDE_DIRS}")
    message("EXECINFO_INCLUDE_DIRS: ${EXECINFO_INCLUDE_DIRS}")
endif()

message("WITH_PYTHON: ${WITH_PYTHON}")
if (WITH_PYTHON)
    message("PYTHON_LIBRARY: ${PYTHON_LIBRARY}")
    message("PYTHON_INCLUDE_PATH: ${PYTHON_INCLUDE_PATH}")
    message("PYTHON_INSTALL_PATH: ${PYTHON_INSTALL_PATH}")

    message("Cython path: ${CYTHON_BIN}")
endif ()

message("WITH_RUBY: ${WITH_RUBY}")
if (WITH_RUBY)
    message("RUBY_INCLUDE_DIRS: ${RUBY_INCLUDE_DIRS}")
    message("RUBY_LIBRARY: ${RUBY_LIBRARY}")
endif ()

message("WITH_ECM: ${WITH_ECM}")
if (WITH_ECM)
    message("ECM_INCLUDE_DIRS: ${ECM_INCLUDE_DIRS}")
    message("ECM_LIBRARIES: ${ECM_LIBRARIES}")
endif()

message("WITH_PRIMESIEVE: ${WITH_PRIMESIEVE}")
if (WITH_PRIMESIEVE)
    message("PRIMESIEVE_INCLUDE_DIRS: ${PRIMESIEVE_INCLUDE_DIRS}")
    message("PRIMESIEVE_LIBRARIES: ${PRIMESIEVE_LIBRARIES}")
endif()

message("WITH_ARB: ${WITH_ARB}")
if (WITH_ARB)
    message("ARB_INCLUDE_DIRS: ${ARB_INCLUDE_DIRS}")
    message("ARB_LIBRARIES: ${ARB_LIBRARIES}")
    message("FLINT_INCLUDE_DIRS: ${FLINT_INCLUDE_DIRS}")
    message("FLINT_LIBRARIES: ${FLINT_LIBRARIES}")
endif()

message("WITH_MPFR: ${WITH_MPFR}")
if (WITH_MPFR)
    message("MPFR_INCLUDE_DIRS: ${MPFR_INCLUDE_DIRS}")
    message("MPFR_LIBRARIES: ${MPFR_LIBRARIES}")
endif()

message("WITH_PIRANHA: ${WITH_PIRANHA}")
if (WITH_PIRANHA)
    message("PIRANHA_INCLUDE_DIRS: ${PIRANHA_INCLUDE_DIRS}")
endif()

message("WITH_BOOST: ${WITH_BOOST}")
if (WITH_BOOST)
    message("BOOST_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message("BOOST_LIBRARIES: ${Boost_LIBRARIES}")
endif()

message("WITH_PTHREAD: ${WITH_PTHREAD}")
if (WITH_PTHREAD)
    message("PTHREAD_INCLUDE_DIRS: ${PTHREAD_INCLUDE_DIRS}")
    message("PTHREAD_LIBRARIES: ${PTHREAD_LIBRARIES}")
endif()

message("WITH_MPC: ${WITH_MPC}")
if (WITH_MPC)
    message("MPC_INCLUDE_DIRS: ${MPC_INCLUDE_DIRS}")
    message("MPC_LIBRARIES: ${MPC_LIBRARIES}")
endif()

message("WITH_TCMALLOC: ${WITH_TCMALLOC}")
if (WITH_TCMALLOC)
    message("TCMALLOC_LIBRARIES: ${TCMALLOC_LIBRARIES}")
endif()

message("WITH_OPENMP: ${WITH_OPENMP}")

message("")
message("--------------------------------------------------------------------------------")
message("")
