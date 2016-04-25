#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

# Shippable currently does not clean the directory after previous builds
# (https://github.com/Shippable/support/issues/238), so
# we need to do it ourselves.
git clean -dfx

if [[ "${TEST_IN_TREE}" != "yes" ]]; then
    mkdir build
    cd build
fi
echo "Current directory:"
export BUILD_DIR=`pwd`
pwd
echo "Running cmake:"
# We build the command line here. If the variable is empty, we skip it,
# otherwise we pass it to cmake.
cmake_line="-DCMAKE_INSTALL_PREFIX=$our_install_dir -DCMAKE_PREFIX_PATH=$our_install_dir"
if [[ "${BUILD_TYPE}" != "" ]]; then
    cmake_line="$cmake_line -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi
if [[ "${WITH_BFD}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_BFD=${WITH_BFD}"
fi
if [[ "${WITH_SYMENGINE_ASSERT}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_SYMENGINE_ASSERT=${WITH_SYMENGINE_ASSERT}"
fi
if [[ "${WITH_SYMENGINE_RCP}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_SYMENGINE_RCP=${WITH_SYMENGINE_RCP}"
fi
if [[ "${WITH_SYMENGINE_THREAD_SAFE}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_SYMENGINE_THREAD_SAFE=${WITH_SYMENGINE_THREAD_SAFE}"
fi
if [[ "${WITH_ECM}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_ECM=${WITH_ECM}"
fi
if [[ "${WITH_PRIMESIEVE}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_PRIMESIEVE=${WITH_PRIMESIEVE}"
fi
if [[ "${WITH_ARB}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_ARB=${WITH_ARB}"
fi
if [[ "${WITH_MPFR}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_MPFR=${WITH_MPFR}"
fi
if [[ "${WITH_MPC}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_MPC=${WITH_MPC}"
fi
if [[ "${WITH_PIRANHA}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_PIRANHA=${WITH_PIRANHA}"
fi
if [[ "${WITH_BENCHMARKS_NONIUS}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_BENCHMARKS_NONIUS=${WITH_BENCHMARKS_NONIUS}"
fi
if [[ "${BUILD_SHARED_LIBS}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}"
fi
if [[ "${WITH_RUBY}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_RUBY=${WITH_RUBY}"
fi
if [[ "${TEST_CPP}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_BENCHMARKS=${TEST_CPP} -DBUILD_TESTS=${TEST_CPP}"
fi
if [[ "${INTEGER_CLASS}" != "" ]]; then
    cmake_line="$cmake_line -DINTEGER_CLASS=${INTEGER_CLASS}"
fi
if [[ "${WITH_COVERAGE}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_COVERAGE=${WITH_COVERAGE}"
fi

if [[ "${CC}" == "clang"* ]] && [[ "${TRAVIS_OS_NAME}" == "linux" ]]; then
    CXXFLAGS=""
else
    CXXFLAGS="-Werror"
fi
cmake $cmake_line ${SOURCE_DIR}

echo "Current directory:"
pwd
echo "Running make:"
make
echo "Running make install:"
make install

if [[ "${TEST_CPP}" == "no" ]]; then
    exit 0;
fi

echo "Running tests in build directory:"
# C++
ctest --output-on-failure

if [[ "${WITH_COVERAGE}" == "yes" ]]; then
    bash <(curl -s https://codecov.io/bash) -x $GCOV_EXECUTABLE
    exit 0;
fi

echo "Running tests using installed SymEngine:"

cd $SOURCE_DIR/benchmarks

compile_flags=`cmake --find-package -DNAME=SymEngine -DSymEngine_DIR=$our_install_dir/lib/cmake/symengine -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=COMPILE`
link_flags=`cmake --find-package -DNAME=SymEngine -DSymEngine_DIR=$our_install_dir/lib/cmake/symengine  -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=LINK`

${CXX} -std=c++0x $compile_flags expand1.cpp $link_flags
export LD_LIBRARY_PATH=$our_install_dir/lib:$LD_LIBRARY_PATH
./a.out

echo "Checking whether all header files are installed:"
python $SOURCE_DIR/symengine/utilities/tests/test_make_install.py $our_install_dir/include/symengine/ $SOURCE_DIR/symengine
