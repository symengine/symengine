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
cmake_line="-DCMAKE_INSTALL_PREFIX=$our_install_dir -DCOMMON_DIR=$common_dir"
if [[ "${BUILD_TYPE}" != "" ]]; then
    cmake_line="$cmake_line -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi
if [[ "${WITH_BFD}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_BFD=${WITH_BFD}"
fi
if [[ "${WITH_PYTHON}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_PYTHON=${WITH_PYTHON}"
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
if [[ "${BUILD_SHARED_LIBS}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}"
fi
if [[ "${WITH_RUBY}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_RUBY=${WITH_RUBY}"
fi
if [[ "${WITH_SAGE}" != "" ]]; then
    cmake_line="$cmake_line -DPYTHON_INSTALL_PATH=$python_install_dir"
fi
if [[ "${PYTHON_INSTALL}" == "yes" ]]; then
    git clean -dfx
    pip install $SOURCE_DIR
    mkdir -p empty
    cd empty
    cat << EOF | python
import symengine
if not symengine.test():
    raise Exception('Tests failed')
EOF
    cd ..
    exit 0
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
echo "Running tests in build directory:"
# C++
ctest --output-on-failure
# Python
if [[ "${WITH_PYTHON}" == "yes" ]] && [[ "${WITH_SAGE}" != "yes" ]]; then
    cd symengine/python
    nosetests -v
    cd ../../
fi
# Ruby
if [[ "${WITH_RUBY}" == "yes" ]]; then
    cd symengine/ruby
    echo "Installing dependent gems"
    bundle install
    echo "Running RSpec tests for Ruby extension in $RUBY_GEM_DIR"
    bundle exec rspec
    cd ../../
fi
echo "Running tests using installed SymEngine:"
# C++
cd $SOURCE_DIR/symengine/tests/basic/
extra_libs=""
if [[ "${WITH_BFD}" != "" ]]; then
    extra_libs="$extra_libs -lbfd"
fi
if [[ "${WITH_ECM}" != "" ]]; then
    extra_libs="$extra_libs -lecm"
fi
if [[ "${WITH_PRIMESIEVE}" != "" ]]; then
    extra_libs="$extra_libs -lprimesieve"
fi
if [[ "${WITH_ARB}" != "" ]]; then
    extra_libs="$extra_libs -larb -lflint"
fi
if [[ "${WITH_MPC}" != "" ]]; then
    extra_libs="$extra_libs -lmpc"
fi
if [[ "${WITH_MPFR}" == "yes" ]] || [[ "${WITH_MPC}" == "yes" ]] || [[ "${WITH_ARB}" == "yes" ]] || [[ "${WITH_PIRANHA}" == "yes" ]]; then
    extra_libs="$extra_libs -lmpfr"
fi

extra_include_dirs="-I$SOURCE_DIR/symengine/teuchos -I$BUILD_DIR/symengine/teuchos -I$SOURCE_DIR/symengine/catch"

${CXX} -std=c++0x -I$our_install_dir/include/ -I$common_dir/include/ $extra_include_dirs -L$our_install_dir/lib -L$common_dir/lib/ -L$BUILD_DIR/symengine/catch test_basic.cpp  -lcatch -lsymengine -lteuchos $extra_libs -lgmpxx -lgmp
export LD_LIBRARY_PATH=$our_install_dir/lib:$LD_LIBRARY_PATH
./a.out
# Python
if [[ "${WITH_PYTHON}" == "yes" ]] && [[ "${WITH_SAGE}" != "yes" ]]; then
    mkdir -p empty
    cd empty
    cat << EOF | python
import symengine
if not symengine.test():
    raise Exception('Tests failed')
EOF
fi
if [[ "${WITH_SAGE}" == "yes" ]]; then
    sage -t $SOURCE_DIR/symengine/python/symengine/tests/test_sage.py
fi
