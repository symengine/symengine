# Install script for directory: /home/akash/symengine/symengine

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/akash/symengine/symengine/libsymengine.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/symengine" TYPE FILE FILES
    "/home/akash/symengine/symengine/add.h"
    "/home/akash/symengine/symengine/symengine_assert.h"
    "/home/akash/symengine/symengine/functions.h"
    "/home/akash/symengine/symengine/monomials.h"
    "/home/akash/symengine/symengine/polynomial.h"
    "/home/akash/symengine/symengine/number.h"
    "/home/akash/symengine/symengine/rings.h"
    "/home/akash/symengine/symengine/basic.h"
    "/home/akash/symengine/symengine/symengine_rcp.h"
    "/home/akash/symengine/symengine/integer.h"
    "/home/akash/symengine/symengine/mul.h"
    "/home/akash/symengine/symengine/pow.h"
    "/home/akash/symengine/symengine/constants.h"
    "/home/akash/symengine/symengine/symbol.h"
    "/home/akash/symengine/symengine/expression.h"
    "/home/akash/symengine/symengine/basic-inl.h"
    "/home/akash/symengine/symengine/dict.h"
    "/home/akash/symengine/symengine/matrix.h"
    "/home/akash/symengine/symengine/ntheory.h"
    "/home/akash/symengine/symengine/rational.h"
    "/home/akash/symengine/symengine/complex.h"
    "/home/akash/symengine/symengine/visitor.h"
    "/home/akash/symengine/symengine/eval_double.h"
    "/home/akash/symengine/symengine/diophantine.h"
    "/home/akash/symengine/symengine/cwrapper.h"
    "/home/akash/symengine/symengine/printer.h"
    "/home/akash/symengine/symengine/real_double.h"
    "/home/akash/symengine/symengine/eval_mpfr.h"
    "/home/akash/symengine/symengine/eval_arb.h"
    "/home/akash/symengine/symengine/eval_mpc.h"
    "/home/akash/symengine/symengine/complex_double.h"
    "/home/akash/symengine/symengine/real_mpfr.h"
    "/home/akash/symengine/symengine/complex_mpc.h"
    "/home/akash/symengine/symengine/type_codes.inc"
    "/home/akash/symengine/symengine/lambda_double.h"
    "/home/akash/symengine/symengine/series.h"
    "/home/akash/symengine/symengine/symengine_config.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/akash/symengine/symengine/utilities/teuchos/cmake_install.cmake")
  INCLUDE("/home/akash/symengine/symengine/utilities/catch/cmake_install.cmake")
  INCLUDE("/home/akash/symengine/symengine/tests/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

