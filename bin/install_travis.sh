#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

# Shippable currently does not clean the directory after previous builds
# (https://github.com/Shippable/support/issues/238), so
# we need to do it ourselves.
git clean -dfx

if [[ "$(uname)" == "Darwin"  ]]; then
    export TRAVIS_OS_NAME="osx"
else
    export TRAVIS_OS_NAME="linux"
fi

if [[ "${CC}" == "" ]]; then
    if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
        export CC=clang
        export CXX=clang++
    else
        export CC=gcc
        export CXX=g++
    fi
fi

if [[ "${CXX}" == "" ]]; then
    if [[ "$CC" == gcc ]]; then
        export CXX=g++
    elif [[ "$CC" == clang ]]; then
        export CXX=clang++
    fi
fi
export GCOV_EXECUTABLE=gcov

if [[ "${TRAVIS_OS_NAME}" == "linux" ]] && [[ "${CC}" == "gcc" ]]; then
    if [[ "${WITH_LATEST_GCC}" == "yes" ]]; then
        export CC=gcc-10
        export CXX=g++-10
        export GCOV_EXECUTABLE=gcov-10
    else
        export CC=gcc-9
        export CXX=g++-9
        export GCOV_EXECUTABLE=gcov-9
    fi
fi

export SOURCE_DIR=`pwd`
export our_install_dir="$HOME/our_usr"

if [[ ! -d $HOME/conda_root/pkgs ]]; then
    rm -rf $HOME/conda_root
    if [[ "${TRAVIS_OS_NAME}" == "osx" ]]; then
        wget -q https://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -O miniconda.sh;
    else
        wget -q https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh;
    fi
    bash miniconda.sh -b -p $HOME/conda_root
fi
export PATH="$HOME/conda_root/bin:$PATH"
conda config --set always_yes yes --set changeps1 no
conda config --add channels conda-forge --force
# Useful for debugging any issues with conda
conda info -a

if [[ "${INTEGER_CLASS}" == "boostmp" ]]; then
    conda_pkgs="$conda_pkgs boost=1.68";
else
    conda_pkgs="$conda_pkgs gmp=6.1.1";
fi

if [[ "${WITH_BENCHMARKS_NONIUS}" == "yes" ]]; then
    conda_pkgs="${conda_pkgs} boost=1.68"
fi

if [[ "${WITH_PIRANHA}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs piranha=0.8 cmake=3.10.0"
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

if [[ "${WITH_FLINT}" == "yes" ]] && [[ "${WITH_FLINT_DEV}" != "yes" ]]; then
    conda_pkgs="$conda_pkgs libflint=2.5.2"
fi

if [[ "${WITH_ARB}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs arb=2.8.1"
fi

if [[ "${WITH_LLVM}" == "12" ]]; then
    export LLVM_DIR=/usr/lib/llvm-12/share/llvm/
    export CC=clang-12
    export CXX=clang++-12
elif [[ "${WITH_LLVM}" == "13" ]]; then
    export LLVM_DIR=/usr/lib/llvm-13/share/llvm/
    export CC=clang-13
    export CXX=clang++-13
elif [[ ! -z "${WITH_LLVM}" ]]; then
    conda_pkgs="$conda_pkgs llvmdev=${WITH_LLVM} cmake=3.10.0"
    export LLVM_DIR=$our_install_dir/share/llvm/
fi

if [[ "${WITH_ECM}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs ecm=7.0.4"
fi

if [[ "${BUILD_DOXYGEN}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs doxygen=1.8.13"
fi

if [[ "${BUILD_TUTORIALS}" == "yes" ]]; then
    conda_pkgs="$conda_pkgs jupytext papermill xeus-cling"
fi

retry_on_error () {
  "$@" || (sleep 5 && "$@") || (sleep 30 && "$@") || (sleep 120 && "$@")
}

if [[ "${CONDA_ENV_FILE}" == "" ]]; then
    retry_on_error conda create -q -p $our_install_dir ${conda_pkgs};
else
    retry_on_error conda env create -q -p $our_install_dir --file ${CONDA_ENV_FILE};
fi
source activate $our_install_dir;

conda install -q ccache

export CXX="ccache ${CXX}"
export CC="ccache ${CC}"
export CCACHE_DIR=$HOME/.ccache
export CCACHE_SLOPPINESS="pch_defines,time_macros"
ccache -M 100M
ccache --version
ccache --zero-stats
ccache --show-stats

if [[ "${WITH_FLINT_DEV}" == "yes" ]] && [[ "${WITH_ARB}" != "yes" ]]; then
    git clone https://github.com/wbhart/flint2;
    cd flint2 && git checkout v2.8.0 && ./configure --prefix=$our_install_dir --with-gmp=$our_install_dir --with-mpfr=$our_install_dir && make -j2 install && cd ..;
fi

cd $SOURCE_DIR;

# Since this script is getting sourced, remove error on exit
set +e
set +x
