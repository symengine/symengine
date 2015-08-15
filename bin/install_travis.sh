#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

if [[ "${CC}" == "" ]]; then
    export CC=gcc
    export CXX=g++
fi
if [[ "${TRAVIS_OS_NAME}" == "osx" ]] && [[ "${CC}" == "gcc" ]]; then
    export CC=gcc-4.8
    export CXX=g++-4.8
fi
if [[ "${WITH_PIRANHA}" == "yes" ]]; then
    export CC=gcc-4.8
    export CXX=g++-4.8
fi

export SOURCE_DIR=`pwd`
export our_install_dir="$HOME/our_usr"
export common_dir=$our_install_dir
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
if [[ "${WITH_ARB}" == "yes" ]]; then
    wget https://raw.githubusercontent.com/symengine/dependencies/6a42d290071921a0a478c6883fc0ddd709d664c9/flint-2.4.4.tar.gz;
    tar -xzf flint-2.4.4.tar.gz;
    cd flint-2.4.4 && ./configure --prefix=$our_install_dir && make -j8 install && cd ..;
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
# Install python using Miniconda.
if [[ "${WITH_PYTHON}" == "yes" && "${WITH_SAGE}" != "yes" || "${PYTHON_INSTALL}" == "yes" ]]; then
    if [[ "${TRAVIS_OS_NAME}" != "osx" ]]; then
        wget https://repo.continuum.io/miniconda/Miniconda-latest-Linux-x86_64.sh -O miniconda.sh;
    else
        wget https://repo.continuum.io/miniconda/Miniconda-latest-MacOSX-x86_64.sh -O miniconda.sh;
    fi
    bash miniconda.sh -b -p $our_install_dir/miniconda;
    export PATH="$our_install_dir/miniconda/bin:$PATH";
    hash -r;
    conda config --set always_yes yes --set changeps1 no;
    conda update -q conda;
    conda info -a;

    conda create -q -n test-environment python="${PYTHON_VERSION}" pip cython sympy nose pytest;
    source activate test-environment;
fi
if [[ "${WITH_SAGE}" == "yes" ]]; then
    wget -O- http://files.sagemath.org/linux/64bit/sage-6.8-x86_64-Linux-Ubuntu_12.04_64_bit.tar.gz | tar xz
    export PATH="`pwd`/sage-6.8-x86_64-Linux:$PATH"
    SAGE_ROOT=$(sage -python << EOF
import os
print os.environ['SAGE_ROOT']
EOF
)
    export python_install_dir=$our_install_dir/lib/python2.7/site-packages
    export SAGE_PATH=$python_install_dir
    source $SAGE_ROOT/src/bin/sage-env
    export common_dir=$SAGE_LOCAL
fi
cd $SOURCE_DIR;
