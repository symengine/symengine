# CSymPy

[![Build Status](https://travis-ci.org/certik/csympy.png?branch=master)](https://travis-ci.org/certik/csympy)

CSymPy is a standalone fast C++ symbolic manipulation library.
Optional thin Python wrappers allow easy usage from Python and
integration with [SymPy](http://sympy.org/).

## License

All files are licensed under MIT license, see the
[LICENSE](https://raw.github.com/certik/csympy/master/LICENSE) for more
information. The `src/teuchos` directory is licensed under the Trilinos BSD
license (see the LICENSE file).

## Installation

Install prerequisites::

For Debian based systems (Ubuntu etc.)

    apt-get install cmake libgmp-dev binutils-dev

For RPM based systems (Fedora etc.)

    yum install cmake gmp-devel binutils-devel

The `binutils-dev`(`binutils-devel`) is optional, but recommended for stacktrace support.

Not installing `binutils-dev` may generate an error during the installation
process. This is because `BFD` in `binutils-dev`(`binutils-devel`) is used for
stacktracing. However, you can avoid this error by disabling `BFD` while
installing.

    cmake -DWITH_BFD=no .

This will disable stacktrace support and is not recommended if you are
interested in development.

Install csympy in Release mode (default):

    cmake .
    make

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
implemented.

### CMake Options

Here are some of the `CMake` options that you can use to configure the build:

    cmake -DCMAKE_INSTALL_PREFIX:PATH="$ARTIFACT" \   # Installation prefix
        -DCMAKE_BUILD_TYPE:STRING="Release" \         # Type of build, one of: Debug or Release
        -DWITH_BFD:BOOL=ON \                          # Install with BFD library (requires binutils-dev)
        -DWITH_PYTHON:BOOL=ON \                       # Build Python wrappers
        -DWITH_CSYMPY_ASSERT:BOOL=OFF \               # Test all CSYMPY_ASSERT statements in the code
        -DWITH_CSYMPY_RCP:BOOL=OFF \                  # Use our faster special implementation of RCP
        .

`CMake` prints the value of its options at the end of the run.
If you want to use a different compiler, do:

    CXX=clang cmake .

and check that CMake picked it up.

## Developer Documentation

Please follow the [C++ Style Guide](doc/style_guide.rst) when developing.

The design decisions are documented in [Design](doc/design.md).

Memory management is handled by RCP (reference counted pointers) from
`Trilinos` (module `Teuchos`). We have copied the relevant files into
`src/teuchos`, so no external dependency is needed.  Brief code snippets of the
most frequent operations are given in our [C++ Style
Guide](doc/style_guide.rst), this is useful to consult if you are unsure about
the syntax.  In order to understand how it works under the hood, read
[Teuchos::RCP Beginner's
Guide](http://trilinos.sandia.gov/RefCountPtrBeginnersGuideSAND.pdf) (pdf).
Finally, more thorough exposition is given in [Teuchos C++ Memory Management
Classes, Idioms, and Related Topics --- The Complete
Reference](http://www.cs.sandia.gov/~rabartl/TeuchosMemoryManagementSAND.pdf)
(pdf).

`Teuchos`' RCP implements reference counting of objects, exactly like Python
works. When an object runs out of scope, its reference count is decreased. When
it is copied, its reference count is increased. When reference count goes to
zero, it is deallocated. This all happens automatically, so as long as our C++
Style Guide is followed, things just work.

When `CMAKE_BUILD_TYPE=Debug` is set in our `CMake` (the default), then
`Teuchos` is compiled with debugging support, which means that as long as you
follow our C++ Style Guide, the C++ code should never segfault (since you never
access raw pointers that could segfault and Teuchos raises nice exception with
full stacktrace if there is any problem, very similar to Python). Use this mode
when developing.

When `CMAKE_BUILD_TYPE=Release`, then `Teuchos` is compiled without debugging
support, which means that all pointer operations become either as fast as raw
pointers, or very close. As such, there is pretty much zero overhead. However,
in this mode, the program can segfault if you access memory incorrectly. This
segfault however would be prevented if `CMAKE_BUILD_TYPE=Debug`, so always use
the Debug build to test your code, only when all tests pass you can enable
Release mode.
