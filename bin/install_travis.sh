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
    brew install gcc48
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


if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
    wget https://repo.continuum.io/miniconda/Miniconda2-latest-MacOSX-x86_64.sh -O miniconda.sh;
else
    wget https://repo.continuum.io/miniconda/Miniconda2-latest-Linux-x86_64.sh -O miniconda.sh;
fi
bash miniconda.sh -b -p $our_install_dir
export PATH="$our_install_dir/bin:$PATH"
conda config --add channels conda-forge --force
cd $our_install_dir

conda_pkgs="$conda_pkgs gmp=6.1.1"

if [[ "${WITH_BENCHMARKS_NONIUS}" == "yes" ]]; then
    conda_pkgs="${conda_pkgs} boost=1.62"
fi

if [[ "${WITH_PIRANHA}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs boost=1.62 cmake=3.6.2 mpfr=3.1.4"
fi

if [[ "${WITH_PRIMESIEVE}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs primesieve=5.6.0"
fi

if [[ "${WITH_MPFR}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs mpfr=3.1.4"
fi

if [[ "${WITH_MPC}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs mpc=1.0.3"
fi

if [[ "${WITH_FLINT}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs libflint=2.5.2"
fi

if [[ "${WITH_ARB}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs arb=2.8.1"
fi

if [[ "${WITH_LLVM}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs llvmdev=3.8 cmake=3.6.2"
fi

conda install -y $conda_pkgs

if [[ "${WITH_ECM}" == "yes" ]]; then
    wget https://raw.githubusercontent.com/symengine/dependencies/6a42d290071921a0a478c6883fc0ddd709d664c9/ecm-6.4.4.tar.gz
    tar -xzf ecm-6.4.4.tar.gz;
    cd ecm-6.4.4 && ./configure --prefix=$our_install_dir --with-gmp=$our_install_dir && make -j8 install && cd ..;
fi

if [[ "${WITH_PIRANHA}" == "yes" ]]; then
    wget https://github.com/bluescarni/piranha/archive/v0.5.tar.gz;
    tar -xzf v0.5.tar.gz;
    cd piranha-0.5 && mkdir build && cd build;
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$our_install_dir -DCMAKE_PREFIX_PATH=$our_install_dir -DBUILD_TESTS=no ../
    make -j8 install && cd ../..;
fi
export LLVM_DIR=$our_install_dir/share/llvm/
cd $SOURCE_DIR;

# Since this script is getting sourced, remove error on exit
set +e
set +x
