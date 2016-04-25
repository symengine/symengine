#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

# Shippable currently does not clean the directory after previous builds
# (https://github.com/Shippable/support/issues/238), so
# we need to do it ourselves.
git clean -dfx

if [[ "${CC}" == "" ]]; then
    export CC=gcc
    export CXX=g++
fi
export GCOV_EXECUTABLE=gcov

if [[ "${TRAVIS_OS_NAME}" == "osx" ]] && [[ "${CC}" == "gcc" ]]; then
    export CC=gcc-4.8
    export CXX=g++-4.8
    export GCOV_EXECUTABLE=gcov-4.8
fi

if [[ "${TRAVIS_OS_NAME}" == "linux" ]] && [[ "${CC}" == "gcc" ]]; then
    if [[ "${WITH_PIRANHA}" == "yes" ]]; then
        export CC=gcc-4.8
        export CXX=g++-4.8
        export GCOV_EXECUTABLE=gcov-4.8
    elif [[ "${WITH_LATEST_GCC}" == "yes" ]]; then
        export CC=gcc-5
        export CXX=g++-5
        export GCOV_EXECUTABLE=gcov-5

    else
        export CC=gcc-4.7
        export CXX=g++-4.7
        export GCOV_EXECUTABLE=gcov-4.7
    fi
fi

export SOURCE_DIR=`pwd`
export our_install_dir="$HOME/our_usr"
mkdir -p $our_install_dir
cd $our_install_dir

if [[ "${TRAVIS_OS_NAME}" != "osx" ]]; then
    if [[ "${TRAVIS}" != "true" ]]; then
        sudo apt-get update
        sudo apt-get install cmake libgmp-dev
    fi
else
    brew install cmake
    wget https://raw.githubusercontent.com/symengine/dependencies/6a42d290071921a0a478c6883fc0ddd709d664c9/gmp-6.0.0a.tar.bz2
    tar -xjf gmp-6.0.0a.tar.bz2;
    cd gmp-6.0.0 && ./configure --prefix=$our_install_dir --enable-cxx && make -j8 install && cd ..;
fi

if [[ "${WITH_BFD}" == "yes" ]]; then
    if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
        brew install binutils;
    elif [[ "${TRAVIS}" != "true" ]]; then
        sudo apt-get install binutils-dev;
    fi
fi
if [[ "${WITH_ECM}" == "yes" ]]; then
    wget https://raw.githubusercontent.com/symengine/dependencies/6a42d290071921a0a478c6883fc0ddd709d664c9/ecm-6.4.4.tar.gz
    tar -xzf ecm-6.4.4.tar.gz;
    cd ecm-6.4.4 && ./configure --prefix=$our_install_dir && make -j8 install && cd ..;
fi
if [[ "${WITH_PRIMESIEVE}" == "yes" ]]; then
    wget https://raw.githubusercontent.com/symengine/dependencies/6a42d290071921a0a478c6883fc0ddd709d664c9/primesieve-5.2.tar.gz
    tar -xzf primesieve-5.2.tar.gz;
    cd primesieve-5.2 && ./configure --prefix=$our_install_dir && make -j8 install && cd ..;
fi
if [[ "${WITH_MPFR}" == "yes" ]] || [[ "${WITH_MPC}" == "yes" ]] || [[ "${WITH_ARB}" == "yes" ]]; then
    if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
        brew install mpfr;
    elif [[ "${TRAVIS}" != "true" ]]; then
        sudo apt-get install libmpfr-dev;
    fi
fi
if [[ "${WITH_ARB}" == "yes" ]] || [[ "${WITH_FLINT}" == "yes" ]]; then
    wget https://raw.githubusercontent.com/symengine/dependencies/6a42d290071921a0a478c6883fc0ddd709d664c9/flint-2.4.4.tar.gz;
    tar -xzf flint-2.4.4.tar.gz;
    cd flint-2.4.4 && ./configure --prefix=$our_install_dir && make -j8 install && cd ..;
fi
if [[ "${WITH_ARB}" == "yes" ]]; then
    wget https://github.com/fredrik-johansson/arb/archive/2.6.0.tar.gz;
    tar -xzf 2.6.0.tar.gz;
    cd arb-2.6.0 && ./configure --prefix=$our_install_dir  --with-flint=$our_install_dir;
    make -j8 install ARB_GMP_LIB_DIR=/usr/lib/x86_64-linux-gnu ARB_MPFR_LIB_DIR=/usr/lib/x86_64-linux-gnu && cd ..;
fi
if [[ "${WITH_MPC}" == "yes" ]]; then
    if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
        brew install libmpc;
    elif [[ "${TRAVIS}" != "true" ]]; then
        sudo apt-get install libmpc-dev;
    fi
fi
if [[ "${WITH_PIRANHA}" == "yes" ]]; then
    git clone https://github.com/bluescarni/piranha;
    cd piranha && mkdir build && cd build;
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$our_install_dir -DBUILD_TESTS=no ../ && make -j8 install && cd ../..;
fi
cd $SOURCE_DIR;
