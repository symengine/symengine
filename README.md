# SymEngine

[![Build Status](https://travis-ci.org/sympy/csympy.png?branch=master)](https://travis-ci.org/sympy/csympy)

SymEngine is a standalone fast C++ symbolic manipulation library.
Optional thin Python wrappers allow easy usage from Python and
integration with [SymPy](http://sympy.org/).

## License

All files are licensed under MIT license, see the [LICENSE](LICENSE) for more
information. The `src/teuchos` directory is licensed under the Trilinos BSD
license (see the LICENSE file).

## Mailinglist, Chat

We use the SymPy mailinglist: http://groups.google.com/group/sympy

[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/sympy/csympy?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Installation

Install prerequisites.
For Debian based systems (Ubuntu etc.):

    apt-get install cmake libgmp-dev

For RPM based systems (Fedora etc.):

    yum install cmake gmp-devel

Install csympy:

    cmake .
    make

This will configure and build SymEngine in the default Release mode with all
code and compiler optimizations on.

Run tests:

    ctest

### Python Wrappers

The optional Python wrappers can be turned on by

    cmake -DWITH_PYTHON=yes .
    make

Use SymEngine from Python as follows:

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
mode you want to develop it. However, the best way to develop SymEngine is to
use the Debug mode, turn assertions on and turn `BFD` support on (prints very
nice stacktraces on exceptions, segfaults or assert errors):

    cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_CSYMPY_ASSERT=yes -DWITH_BFD=yes .

To make `WITH_BFD=yes` work, you need to install `binutils-dev` first,
otherwise you will get a `CMake` error during configuring.
For Debian based systems (Ubuntu etc.)

    apt-get install binutils-dev

For RPM based systems (Fedora etc.)

    yum install binutils-devel

## CMake Options

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

### External Libraries

There are three ways how to specify where external libraries are. In the lines
below, change `PKG1`, `PKG2`, ... to the names of the external libraries (`GMP`, `ARB`, `PRIMESIEVE`,
`BFD`, `FLINT`, `MPFR`, ...).

1. `cmake -DPKG1_DIR=$HASHSTACK -DPKG2_DIR=$HASHSTACK .`
2. `cmake -DPKG1_INCLUDE_DIRS=$HASHSTACK/include -DPKG1_LIBRARIES=$HASHSTACK/lib -DPKG2_DIR=$HASHSTACK .`
3. `cmake -DCOMMON_DIR=$HASHSTACK .`

In the approach 1., you specify `PKG_DIR` as the base prefix, and the include
files must be in `${PKG_DIR}/include` and libraries in `${PKG_DIR}/lib` (or
`lib64`). In the approach 2., you specify the include and library directories
separately (you can use approach 1. for some libraries and 2. for other
libraries on the same command line). In the approach 3., you specify a common
prefix for all libraries at once.

If all your libraries are installed in the same prefix, use 3. If they are
installed in separate locations, use 1. or 2.: if the given library has a
common prefix for includes and libs, use 1., otherwise use 2.

## Developer Documentation

Please follow the [C++ Style Guide](doc/style_guide.md) when developing.

The design decisions are documented in [Design](doc/design.md).
