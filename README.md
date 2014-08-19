# CSymPy

[![Build Status](https://travis-ci.org/sympy/csympy.png?branch=master)](https://travis-ci.org/sympy/csympy)

CSymPy is a standalone fast C++ symbolic manipulation library.
Optional thin Python wrappers allow easy usage from Python and
integration with [SymPy](http://sympy.org/).

## License

All files are licensed under MIT license, see the [LICENSE](LICENSE) for more
information. The `src/teuchos` directory is licensed under the Trilinos BSD
license (see the LICENSE file).

## Mailinglist, Chat

We use the SymPy mailinglist: http://groups.google.com/group/sympy

Chat (Gitter): https://gitter.im/sympy/csympy

## Installation

Install prerequisites.
For Debian based systems (Ubuntu etc.):

    apt-get install cmake libgmp-dev

For RPM based systems (Fedora etc.):

    yum install cmake gmp-devel

Install csympy:

    cmake .
    make

This will configure and build CSymPy in the default Release mode with all code
and compiler optimizations on.

Run tests:

    ctest

### Python Wrappers

The optional Python wrappers can be turned on by

    cmake -DWITH_PYTHON=yes .
    make

Use CSymPy from Python as follows:

    >>> from csympy import var
    >>> var("x y z")
    (x, y, z)
    >>> e = (x+y+z)**2
    >>> e.expand()
    2x*z + x^2 + 2y*x + 2y*z + z^2 + y^2

You can read Python tests in `csympy/tests` to see what features are
implemented. Supported versions of Python are: 2.6, 2.7, 3.2, 3.3.
You need Cython >= 0.19.1 in order to compile the wrappers. CMake
will report at configure time if the Cython version is too old.

### Development

The Travis-CI checks the code in both Release and Debug mode with all possible
checks, so just sending a GitHub pull request is enough and you can use any
mode you want to develop it. However, the best way to develop CSymPy is to use
the Debug mode, turn assertions on and turn `BFD` support on (prints very nice
stacktraces on exceptions, segfaults or assert errors):

    cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_CSYMPY_ASSERT=yes -DWITH_BFD=yes .

To make `WITH_BFD=yes` work, you need to install `binutils-dev` first,
otherwise you will get a `CMake` error during configuring.
For Debian based systems (Ubuntu etc.)

    apt-get install binutils-dev

For RPM based systems (Fedora etc.)

    yum install binutils-devel

### CMake Options

Here are all the `CMake` options that you can use to configure the build, with
their default values indicated below:

    cmake -DCMAKE_INSTALL_PREFIX:PATH="/usr/local" \  # Installation prefix
        -DCMAKE_BUILD_TYPE:STRING="Release" \         # Type of build, one of: Debug or Release
        -DWITH_BFD:BOOL=OFF \                         # Install with BFD library (requires binutils-dev)
        -DWITH_PYTHON:BOOL=OFF \                      # Build Python wrappers
        -DWITH_CSYMPY_ASSERT:BOOL=OFF \               # Test all CSYMPY_ASSERT statements in the code
        -DWITH_CSYMPY_RCP:BOOL=ON \                   # Use our faster special implementation of RCP
        -DWITH_PRIMESIEVE=OFF \                       # Install with Primesieve library
        -DWITH_ARB=OFF \                              # Install with ARB library
        .

`CMake` prints the value of its options at the end of the run.
If you want to use a different compiler, do:

    CXX=clang cmake .

and check that CMake picked it up.

## Developer Documentation

Please follow the [C++ Style Guide](doc/style_guide.md) when developing.

The design decisions are documented in [Design](doc/design.md).
