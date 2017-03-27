# SymEngine

[![Build Status](https://travis-ci.org/symengine/symengine.png?branch=master)](https://travis-ci.org/symengine/symengine)
[![Build status](https://ci.appveyor.com/api/projects/status/n7vqq2ilkw59o4bx/branch/master?svg=true)](https://ci.appveyor.com/project/certik/symengine/branch/master)
[![codecov.io](https://codecov.io/github/symengine/symengine/coverage.svg?branch=master)](https://codecov.io/github/symengine/symengine?branch=master)

SymEngine is a standalone fast C++ symbolic manipulation library. Optional thin
wrappers allow usage of the library from other languages, e.g.:

* C wrappers allow usage from C, or as a basis for other wrappers (the [symengine/cwrapper.h](https://github.com/sympy/symengine/tree/master/symengine/cwrapper.h) file)
* Python wrappers allow easy usage from Python and integration with [SymPy](http://sympy.org/) and [Sage](http://www.sagemath.org/) (the [symengine.py](https://github.com/symengine/symengine.py) repository)
* Ruby wrappers (the [symengine.rb](https://github.com/symengine/symengine.rb) repository)
* Julia wrappers (the [SymEngine.jl](https://github.com/symengine/SymEngine.jl) repository)
* Haskell wrappers (the [symengine.hs](https://github.com/symengine/symengine.hs) repository)
* ...

## License

All files are licensed under MIT license, see the [LICENSE](LICENSE) for more
information. Third party code packaged are licensed under BSD 3-clause license
(see the LICENSE file).

## Mailinglist, Chat

SymEngine mailinglist: http://groups.google.com/group/symengine

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/sympy/symengine?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Installation

### Ubuntu package manager

    add-apt-repository ppa:symengine/ppa
    apt-get update
    apt-get install libsymengine-dev

### Conda package manager

    conda install symengine -c symengine -c conda-forge

### Homebrew pacakge manager

    brew install homebrew/science/symengine

### Building from source

Install prerequisites.
For Debian based systems (Ubuntu etc.):

    apt-get install cmake libgmp-dev

For RPM based systems (Fedora etc.):

    yum install cmake gmp-devel

Install SymEngine:

    cmake .
    make
    make install

This will configure and build SymEngine in the default Release mode with all
code and compiler optimizations on and then install it on your system.

Run tests:

    ctest

### Development

The Travis-CI checks the code in both Release and Debug mode with all possible
checks, so just sending a GitHub pull request is enough and you can use any
mode you want to develop it. However, the best way to develop SymEngine on Linux
is to use the Debug mode with `BFD` support on:

    cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_BFD=yes .

This `BFD` support turns on nice Python like stacktraces on exceptions, assert
errors or segfaults, and the Debug mode automatically turns on
`WITH_SYMENGINE_RCP=no` (which uses `Teuchos::RCP` with full Debug time
checking) and `WITH_SYMENGINE_ASSERT=yes`, so the code cannot segfault in Debug
mode, as long as our style conventions (e.g. no raw pointers) are followed,
which is easy to check by visual inspection of a given Pull Request. In Release
mode, which is the default, the code is as performing as manual reference
counting and raw pointers (and if there is a bug, it could segfault, in which
case all you have to do is to turn Debug mode on and get a nice exception with
a stacktrace).

To make `WITH_BFD=yes` work, you need to install `binutils-dev` first,
otherwise you will get a `CMake` error during configuring.
For Debian based systems (Ubuntu etc.)

    apt-get install binutils-dev

For RPM based systems (Fedora etc.)

    yum install binutils-devel

On OpenSuSE you will additionally need `glibc-devel`.

## CMake Options

Here are all the `CMake` options that you can use to configure the build, with
their default values indicated below:

    cmake -DCMAKE_INSTALL_PREFIX:PATH="/usr/local" \  # Installation prefix
        -DCMAKE_BUILD_TYPE:STRING="Release" \         # Type of build, one of: Debug or Release
        -DWITH_BFD:BOOL=OFF \                         # Install with BFD library (requires binutils-dev)s
        -DWITH_SYMENGINE_ASSERT:BOOL=OFF \            # Test all SYMENGINE_ASSERT statements in the code
        -DWITH_SYMENGINE_RCP:BOOL=ON \                # Use our faster special implementation of RCP
        -DWITH_SYMENGINE_THREAD_SAFE:BOOL=OFF \       # Build with thread safety
        -DWITH_ECM:BOOL=OFF \                         # Build with GMP-ECM library for integer factorization
        -DWITH_PRIMESIEVE:BOOL=OFF \                  # Install with Primesieve library
        -DWITH_FLINT:BOOL=OFF \                       # Install with Flint library
        -DWITH_ARB:BOOL=OFF \                         # Install with ARB library
        -DWITH_TCMALLOC:BOOL=OFF \                    # Install with TCMalloc linked
        -DWITH_OPENMP:BOOL=OFF \                      # Install with OpenMP enabled
        -DWITH_PIRANHA:BOOL=OFF \                     # Install with Piranha library
        -DWITH_MPFR:BOOL=OFF \                        # Install with MPFR library
        -DWITH_MPC:BOOL=OFF \                         # Install with MPC library
        -DWITH_LLVM:BOOL=OFF \                        # Build with LLVM libraries
        -DBUILD_TESTS:BOOL=ON \                       # Build with tests
        -DBUILD_BENCHMARKS:BOOL=ON \                  # Build with benchmarks
        -DBUILD_BENCHMARKS_NONIUS:BOOL=OFF \          # Build with Nonius benchmarks
        -DINTEGER_CLASS:STRING=gmp \                  # Choose storage type for Integer. one of gmp, gmpxx,
                                                        flint, piranha, boostmp
        -DBUILD_SHARED_LIBS:BOOL=OFF \                # Build a shared library.
        -DCMAKE_INSTALL_RPATH_USE_LINK_PATH:BOOL=OFF\ # Add dependencies to rpath when a shared lib is built
        .

If `OPENMP` is enabled, then `SYMENGINE_THREAD_SAFE` is also enabled automatically
irrespective of the user input for `WITH_SYMENGINE_THREAD_SAFE`.

`CMake` prints the value of its options at the end of the run.
If you want to use a different compiler, do:

    CC=clang CXX=clang++ cmake .

and check that CMake picked it up.

Using `INTEGER_CLASS=boostmp` would remove the dependency on gmp and use boost's
multiprecision integer and rational classes. This would make boost, the only
dependency and all the code would be under permissive licenses, namely, MIT,
BSD 3-clause and Boost License.

The Nonius based benchmarks (`BUILD_BENCHMARKS_NONIUS`) and Piranha
(`WITH_PIRANHA`) depend on Boost, so they are off by default. The bechmarked
code (both with and without Nonius) seems to depend on the order of which you
execute the benchmarks in a given executable, due to internal malloc
implementation. We have found that this order dependence is reduced by enabling
`WITH_TCMALLOC=ON` and since it also speeds the benchmarks up, we recommend
to always use TCMalloc when benchmarking (and the `Release` mode of SymEngine,
which is the default).

### External Libraries

Use `CMAKE_PREFIX_PATH` to specify the prefixes of the external libraries.

    cmake -DCMAKE_PREFIX_PATH=<prefix1>;<prefix2>

If the headers and libs are not in `<prefix>/include` and `<prefix>/lib` respectively,
use `CMAKE_LIBRARY_PATH` and `CMAKE_INCLUDE_PATH`.

If CMake still cannot find the library, you can specify the path to the library by
doing `cmake -DPKG_LIBRARY=/path/libname.so .`, where `PKG` should be replaced
with the name of the external library (`GMP`, `ARB`, `BFD`, `FLINT`, `MPFR`, ...).
Similarly, `-DPKG_INCLUDE_DIR` can be used for headers.

## Developer Documentation

Please follow the [C++ Style Guide](doc/style_guide.md) when developing.

The design decisions are documented in [Design](doc/design.md).
