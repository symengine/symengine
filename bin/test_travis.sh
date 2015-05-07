#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

# Shippable currently does not clean the directory after previous builds
# (https://github.com/Shippable/support/issues/238), so
# we need to do it ourselves.
git clean -dfx

export SOURCE_DIR=`pwd`
if [[ "${TEST_IN_TREE}" != "yes" ]]; then
    mkdir build
    cd build
fi
echo "Current directory:"
pwd
echo "Running cmake:"
# We build the command line here. If the variable is empty, we skip it,
# otherwise we pass it to cmake.
our_install_dir="$SOURCE_DIR/our_usr"
cmake_line="-DCMAKE_INSTALL_PREFIX=$our_install_dir"
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
if [[ "${BUILD_SHARED_LIBS}" != "" ]]; then
    cmake_line="$cmake_line -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}"
fi
if [[ "${PYTHON_INSTALL}" == "yes" ]]; then
    git clean -dfx
    pip install $SOURCE_DIR
    mkdir -p empty
    cd empty
    cat << EOF | python
import csympy
if not csympy.test():
    raise Exception('Tests failed')
EOF
    cd ..
    exit 0
fi

CXXFLAGS="-Werror" cmake $cmake_line ${SOURCE_DIR}
echo "Current directory:"
pwd
echo "Running make:"
make
echo "Running make install:"
make install

if [[ "${WITH_SYMENGINE_RCP}" == "no" ]]; then
    echo "SymEngine successfully built with Teuchos::RCP. No tests being run."
else
    echo "Running tests in build directory:"
    # C++
    ctest --output-on-failure
    # Python
    if [[ "${WITH_PYTHON}" == "yes" ]]; then
        nosetests -v
    fi

    echo "Running tests using installed SymEngine:"
    # C++
    cd $SOURCE_DIR/src/tests/basic/
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
    ${CXX} -std=c++0x -I$our_install_dir/include/ -L$our_install_dir/lib test_basic.cpp -lcsympy -lteuchos -lgmpxx -lgmp $extra_libs
    export LD_LIBRARY_PATH=/usr/local/lib:$our_install_dir/lib:$LD_LIBRARY_PATH
    ./a.out
    # Python
    if [[ "${WITH_PYTHON}" == "yes" ]]; then
        mkdir -p empty
        cd empty
        cat << EOF | python
import csympy
if not csympy.test():
    raise Exception('Tests failed')
EOF
    fi
fi
