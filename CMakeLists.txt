cmake_minimum_required(VERSION 2.8.5)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_SOURCE_DIR}/cmake/UserOverride.cmake)

project(symengine)
set(SYMENGINE_MAJOR_VERSION 0)
set(SYMENGINE_MINOR_VERSION 1)
set(SYMENGINE_PATCH_VERSION 0)
set(SYMENGINE_VERSION ${SYMENGINE_MAJOR_VERSION}.${SYMENGINE_MINOR_VERSION}.${SYMENGINE_PATCH_VERSION})

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
try_compile(CXX11 "${CMAKE_CURRENT_BINARY_DIR}/cxx" "${CMAKE_SOURCE_DIR}/cmake/checkcxx11.cpp"
	CMAKE_FLAGS "-DCOMPILE_DEFINITIONS=${CMAKE_CXX_FLAGS_RELEASE}" OUTPUT_VARIABLE CXX11_ERROR_LOG)
if (NOT ${CXX11})
	message(FATAL_ERROR "Compiler does not support C++11 constructs. \n"
		"CXX11 Error Log : \n${CXX11_ERROR_LOG}")
endif()

set(HAVE_GCC_ABI_DEMANGLE yes
    CACHE BOOL "Build with C++ name demangling support")

set(HAVE_C_FUNCTION_NOT_FUNC no
    CACHE BOOL "C compiler supports __FUNCTION__ but not __func__")

set(HAVE_DEFAULT_CONSTRUCTORS yes
    CACHE BOOL "C++ compiler supports default constructors")

set(HAVE_SYMENGINE_NOEXCEPT yes
    CACHE BOOL "C++ compiler supports noexcept specifier")

set(HAVE_SYMENGINE_IS_CONSTRUCTIBLE yes
    CACHE BOOL "C++ compiler supports std::is_constructible")

set(HAVE_SYMENGINE_RESERVE yes
    CACHE BOOL "C++ compiler supports std::unordered_map<>::reserve()")

if (MSVC)
    # MSVC doesn't have cxxabi.h
    set(HAVE_GCC_ABI_DEMANGLE no)

    # MSVC only supports __func__ in a C++ mode, not a C mode
    set(HAVE_C_FUNCTION_NOT_FUNC yes)

    # MSVC does not support =default for constructors
    set(HAVE_DEFAULT_CONSTRUCTORS no)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(HAVE_DEFAULT_CONSTRUCTORS no)
    set(HAVE_SYMENGINE_NOEXCEPT no)
    set(HAVE_SYMENGINE_IS_CONSTRUCTIBLE no)
    set(HAVE_SYMENGINE_RESERVE no)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "PGI")
    set(HAVE_DEFAULT_CONSTRUCTORS no)
    set(HAVE_SYMENGINE_RESERVE no)
endif()

if ((CMAKE_CXX_COMPILER_ID MATCHES Clang) AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    try_compile(CHECK_CLANG "${CMAKE_CURRENT_BINARY_DIR}/cxx" "${CMAKE_SOURCE_DIR}/cmake/checkclang.cpp")
    if (NOT ${CHECK_CLANG})
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D__extern_always_inline=inline" )
    endif()
endif()


set(INTEGER_CLASS "gmpxx"
    CACHE STRING "Integer class for symengine. Either gmpxx or piranha")

# Piranha
set(WITH_PIRANHA no CACHE BOOL "Build with Piranha")

# FLINT
set(WITH_FLINT no CACHE BOOL "Build with Flint")
set(HAVE_SYMENGINE_FLINT False)

if ("${INTEGER_CLASS}" MATCHES "piranha")
    set(SYMENGINE_INTEGER_CLASS "PIRANHA")
    set(WITH_PIRANHA yes)
elseif ("${INTEGER_CLASS}" MATCHES "flint")
    set(SYMENGINE_INTEGER_CLASS "FLINT")
    set(WITH_FLINT yes)
elseif ("${INTEGER_CLASS}" MATCHES "gmpxx")
    set(SYMENGINE_INTEGER_CLASS "GMPXX")
    set(WITH_GMPXX yes)
else()
    set(SYMENGINE_INTEGER_CLASS "GMP")
endif()

# GMP
find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GMP_TARGETS})
set(PKGS ${PKGS} "GMP")

# Check gmpxx
if (NOT(MSVC) AND WITH_GMPXX)
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
    set(LIBS ${LIBS} ${ECM_TARGETS})
    set(HAVE_SYMENGINE_ECM yes)
    set(PKGS ${PKGS} "ECM")
endif()

# PRIMESIEVE
set(WITH_PRIMESIEVE no
    CACHE BOOL "Build with primesieve")

if (WITH_PRIMESIEVE)
    find_package(PRIMESIEVE REQUIRED)
    include_directories(${PRIMESIEVE_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${PRIMESIEVE_TARGETS})
    set(HAVE_SYMENGINE_PRIMESIEVE yes)
    set(PKGS ${PKGS} "PRIMESIEVE")
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

if (WITH_FLINT)
    find_package(FLINT REQUIRED)
    include_directories(${FLINT_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${FLINT_TARGETS})
    set(HAVE_SYMENGINE_FLINT True)
    set(PKGS ${PKGS} "FLINT")

    # Workaround for https://github.com/fredrik-johansson/arb/issues/24
    include_directories(${FLINT_INCLUDE_DIRS}/flint)

    set(WITH_MPFR yes)
endif()

if (WITH_ARB)
    find_package(FLINT REQUIRED)
    include_directories(${FLINT_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${FLINT_TARGETS})
    set(PKGS ${PKGS} "FLINT")

    # Workaround for https://github.com/fredrik-johansson/arb/issues/24
    include_directories(${FLINT_INCLUDE_DIRS}/flint)

    set(WITH_FLINT yes)
    set(HAVE_SYMENGINE_FLINT True)
    set(WITH_MPFR yes)

    find_package(ARB REQUIRED)
    include_directories(${ARB_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${ARB_TARGETS})
    set(HAVE_SYMENGINE_ARB True)
    set(PKGS ${PKGS} "ARB")
endif()

if (WITH_MPC)
    find_package(MPC REQUIRED)
    include_directories(${MPC_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${MPC_TARGETS})
    set(HAVE_SYMENGINE_MPC True)
    set(WITH_MPFR yes)
    set(PKGS ${PKGS} "MPC")
endif()

# Boost
set(WITH_BOOST no
    CACHE BOOL "Build with Boost")

# PTHREAD
set(WITH_PTHREAD no
    CACHE BOOL "Build with PTHREAD")

if (WITH_PIRANHA)
    set(WITH_PTHREAD yes)
    set(WITH_MPFR yes)
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

    find_package(PIRANHA REQUIRED)
    include_directories(${PIRANHA_INCLUDE_DIRS})
    set(HAVE_SYMENGINE_PIRANHA yes)
    set(PKGS ${PKGS} "PIRANHA")
    set(BOOST_COMPONENTS ${BOOST_COMPONENTS} serialization iostreams)
    set(WITH_BOOST yes)
endif()

# BENCHMARKS
set(BUILD_BENCHMARKS yes
    CACHE BOOL "Build SymEngine benchmarks")

set(BUILD_BENCHMARKS_NONIUS no
    CACHE BOOL "Build SymEngine Nonius benchmarks")

if (BUILD_BENCHMARKS)
    if (BUILD_BENCHMARKS_NONIUS)
        if(NOT WIN32)
            set(WITH_PTHREAD yes)
        endif()
        set(WITH_BOOST yes)
    endif()
endif()

if (WITH_PTHREAD)
    find_package(PTHREAD REQUIRED)
    include_directories(${PTHREAD_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${PTHREAD_TARGETS})
    set(HAVE_SYMENGINE_PTHREAD yes)
    set(PKGS ${PKGS} "PTHREAD")
endif()

if (WITH_BOOST)
    find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
    include_directories(${Boost_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${Boost_LIBRARIES})
    set(HAVE_SYMENGINE_BOOST yes)
endif()

if (WITH_MPFR)
    find_package(MPFR REQUIRED)
    include_directories(${MPFR_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${MPFR_TARGETS})
    set(HAVE_SYMENGINE_MPFR True)
    set(PKGS ${PKGS} "MPFR")
endif()

# BFD
set(WITH_BFD no
    CACHE BOOL "Build with BFD (binutils-dev) support")

if (WITH_BFD)
    find_package(BFD REQUIRED)
    find_package(LINKH REQUIRED)
    include_directories(${BFD_INCLUDE_DIRS} ${LINKH_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${BFD_TARGETS})
    set(HAVE_TEUCHOS_BFD yes)
    set(HAVE_TEUCHOS_LINK yes)

    set(HAVE_TEUCHOS_EXECINFO yes)
    find_package(EXECINFO REQUIRED)
    include_directories(${EXECINFO_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${EXECINFO_TARGETS})
    set(PKGS ${PKGS} "BFD" "LINKH" "EXECINFO")
endif()

# TCMalloc
set(WITH_TCMALLOC no
    CACHE BOOL "Build with TCMalloc linked")

if (WITH_TCMALLOC)
    find_package(TCMALLOC REQUIRED)
    set(LIBS ${LIBS} ${TCMALLOC_TARGETS})

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
        set(CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
    endif()
    set(PKGS ${PKGS} "TCMALLOC")

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

# Teuchos
set(WITH_SYMENGINE_TEUCHOS no
    CACHE BOOL "Build with teuchos")

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

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # In Debug mode we use Teuchos::RCP and enable debugging checks that make
    # the usage 100% safe, as long as the Teuchos guidelines are followed.
    set(WITH_SYMENGINE_RCP no) # Use the Teuchos::RCP
    set(HAVE_TEUCHOS_DEBUG yes) # Enable safety checks
    set(HAVE_TEUCHOS_DEBUG_RCP_NODE_TRACING yes) # Enable safety checks

    set(WITH_SYMENGINE_ASSERT yes) # Also enable assertions
endif()

if (WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
endif()

if ((NOT WITH_SYMENGINE_RCP) OR HAVE_TEUCHOS_BFD)
    set(WITH_SYMENGINE_TEUCHOS yes)
endif()

enable_testing()
add_subdirectory(symengine)

set(SymEngine_DIR ${PROJECT_BINARY_DIR} CACHE BOOL "SymEngine Build Directory")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKE_DIR CMake)
    set(DEF_INSTALL_CMAKE_DIR_REL ..)
else()
    set(DEF_INSTALL_CMAKE_DIR lib/cmake/symengine)
    set(DEF_INSTALL_CMAKE_DIR_REL ../../..)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
    "Installation directory for CMake files")

if (TARGET teuchos)
    set(SYMENGINE_TARGETS symengine teuchos)
else()
    set(SYMENGINE_TARGETS symengine)
endif()

export(TARGETS ${SYMENGINE_TARGETS} FILE "${PROJECT_BINARY_DIR}/SymEngineTargets.cmake")
foreach(PKG ${PKGS})
    set(SYMENGINE_PKG_FINDS "${SYMENGINE_PKG_FINDS}\nset(SYMENGINE_${PKG}_LIBRARIES ${${PKG}_LIBRARIES})")
    set(SYMENGINE_PKG_FINDS "${SYMENGINE_PKG_FINDS}\nset(SYMENGINE_${PKG}_INCLUDE_DIRS ${${PKG}_INCLUDE_DIRS})")
endforeach()

# ... for the build tree
set(CONF_INCLUDE_DIRS ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
if (TARGET teuchos)
    set(CONF_INCLUDE_DIRS ${CONF_INCLUDE_DIRS}
                           ${PROJECT_SOURCE_DIR}/symengine/utilities/teuchos ${PROJECT_BINARY_DIR}/symengine/utilities/teuchos)
endif()
set(SYMENGINE_BUILD_TREE yes)
configure_file(cmake/SymEngineConfig.cmake.in
    "${PROJECT_BINARY_DIR}/SymEngineConfig.cmake" @ONLY)

# ... for the install tree
set(SYMENGINE_BUILD_TREE no)
configure_file(cmake/SymEngineConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/SymEngineConfig.cmake" @ONLY)

#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#    "${CMAKE_CURRENT_BINARY_DIR}/SymEngineConfigVersion.cmake"
#    VERSION ${SYMENGINE_VERSION}
#    COMPATIBILITY AnyNewerVersion
#)

configure_file(cmake/SymEngineConfigVersion.cmake.in SymEngineConfigVersion.cmake @ONLY)

install(FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/SymEngineConfig.cmake"
              ${PROJECT_BINARY_DIR}/SymEngineConfigVersion.cmake
              cmake/FindFLINT.cmake
              cmake/FindARB.cmake
              cmake/FindBFD.cmake
              cmake/FindECM.cmake
              cmake/FindEXECINFO.cmake
              cmake/FindFLINT.cmake
              cmake/FindGMP.cmake
              cmake/FindLINKH.cmake
              cmake/FindMPC.cmake
              cmake/FindMPFR.cmake
              cmake/FindPIRANHA.cmake
              cmake/FindPRIMESIEVE.cmake
              cmake/FindPTHREAD.cmake
              cmake/FindTCMALLOC.cmake
              cmake/LibFindMacros.cmake
        DESTINATION ${INSTALL_CMAKE_DIR})
install(EXPORT SymEngineTargets DESTINATION ${INSTALL_CMAKE_DIR})

file(COPY ${symengine_SOURCE_DIR}/cmake/ DESTINATION ${symengine_BINARY_DIR}/cmake)

if (BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
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

message("INTEGER_CLASS : ${SYMENGINE_INTEGER_CLASS}")
message("WITH_SYMENGINE_ASSERT: ${WITH_SYMENGINE_ASSERT}")
message("WITH_SYMENGINE_RCP: ${WITH_SYMENGINE_RCP}")
message("WITH_SYMENGINE_TEUCHOS: ${WITH_SYMENGINE_TEUCHOS}")
if (NOT WITH_SYMENGINE_RCP)
    message("HAVE_TEUCHOS_DEBUG: ${HAVE_TEUCHOS_DEBUG}")
    message("HAVE_TEUCHOS_DEBUG_RCP_NODE_TRACING: ${HAVE_TEUCHOS_DEBUG_RCP_NODE_TRACING}")
endif ()
message("HAVE_GCC_ABI_DEMANGLE: ${HAVE_GCC_ABI_DEMANGLE}")
message("HAVE_C_FUNCTION_NOT_FUNC: ${HAVE_C_FUNCTION_NOT_FUNC}")
message("HAVE_DEFAULT_CONSTRUCTORS: ${HAVE_DEFAULT_CONSTRUCTORS}")
message("HAVE_SYMENGINE_NOEXCEPT: ${HAVE_SYMENGINE_NOEXCEPT}")
message("HAVE_SYMENGINE_IS_CONSTRUCTIBLE: ${HAVE_SYMENGINE_IS_CONSTRUCTIBLE}")
message("HAVE_SYMENGINE_RESERVE: ${HAVE_SYMENGINE_RESERVE}")
message("WITH_SYMENGINE_THREAD_SAFE: ${WITH_SYMENGINE_THREAD_SAFE}")
message("BUILD_TESTS: ${BUILD_TESTS}")
message("BUILD_BENCHMARKS: ${BUILD_BENCHMARKS}")
message("BUILD_BENCHMARKS_NONIUS: ${BUILD_BENCHMARKS_NONIUS}")

message("GMP_INCLUDE_DIRS: ${GMP_INCLUDE_DIRS}")
message("GMP_LIBRARIES: ${GMP_LIBRARIES}")

message("WITH_BFD: ${WITH_BFD}")
if (WITH_BFD)
    message("BFD_INCLUDE_DIRS: ${BFD_INCLUDE_DIRS}")
    message("BFD_LIBRARIES: ${BFD_LIBRARIES}")
    message("LINKH_INCLUDE_DIRS: ${LINKH_INCLUDE_DIRS}")
    message("EXECINFO_INCLUDE_DIRS: ${EXECINFO_INCLUDE_DIRS}")
endif()

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

message("WITH_FLINT: ${WITH_FLINT}")
if (WITH_FLINT)
    message("FLINT_INCLUDE_DIRS: ${FLINT_INCLUDE_DIRS}")
    message("FLINT_LIBRARIES: ${FLINT_LIBRARIES}")
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
