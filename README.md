# CSymPy

[![Build Status](https://travis-ci.org/certik/csympy.png?branch=master)](https://travis-ci.org/certik/csympy)

Experimental [SymPy](http://sympy.org/) core written in C++.

## Installation

Install prerequisites::

    apt-get install cmake libgmp-dev binutils-dev

The `binutils-dev` is optional, but recommended for stacktrace support.

Install csympy::

    cmake .
    make

Run tests:

    ctest

### CMake Options

Here are some of the `CMake` options that you can use to configure the build:

    cmake -DCMAKE_INSTALL_PREFIX:PATH="$ARTIFACT" \   # Installation prefix
        -DCMAKE_BUILD_TYPE:STRING="Release" \         # Type of build, one of: Debug or Release
        -DWITH_BFD:BOOL=ON \                          # Install with BFD library (requires binutils-dev)
        .

`CMake` prints the value of its options at the end of the run.
