#!/usr/bin/env bash

sudo apt-get update
sudo apt-get install cmake libgmp-dev
if [[ "${WITH_BFD}" == "yes" ]]; then
    sudo apt-get install binutils-dev;
fi
if [[ "${WITH_ECM}" == "yes" ]]; then
    sudo apt-get install libecm-dev;
fi
if [[ "${WITH_PRIMESIEVE}" == "yes" ]]; then
    wget http://dl.bintray.com/kimwalisch/primesieve/primesieve-5.2.tar.gz;
    tar -xzf primesieve-5.2.tar.gz;
    cd primesieve-5.2 && ./configure && make -j8 && sudo make install && cd ..;
fi
if [[ "${WITH_ARB}" == "yes" ]]; then
    sudo apt-get install libmpfr-dev;
    wget http://www.flintlib.org/flint-2.4.4.tar.gz;
    tar -xzf flint-2.4.4.tar.gz;
    cd flint-2.4.4 && ./configure && make -j8 && sudo make install && cd ..;
    wget https://github.com/fredrik-johansson/arb/archive/2.2.0.tar.gz;
    tar -xzf 2.2.0.tar.gz;
    cd arb-2.2.0 && ./configure && make -j8 ARB_GMP_LIB_DIR=/usr/lib/x86_64-linux-gnu ARB_MPFR_LIB_DIR=/usr/lib/x86_64-linux-gnu && sudo make ARB_GMP_LIB_DIR=/usr/lib/x86_64-linux-gnu ARB_MPFR_LIB_DIR=/usr/lib/x86_64-linux-gnu install && cd ..;
fi
# Install python using Miniconda.
if [[ "${WITH_PYTHON}" == "yes" ]] || [[ "${PYTHON_INSTALL}" == "yes" ]]; then
    wget http://repo.continuum.io/miniconda/Miniconda3-3.7.3-Linux-x86_64.sh -O miniconda.sh;
    bash miniconda.sh -b -p $HOME/miniconda;
    export PATH="$HOME/miniconda/bin:$PATH";
    hash -r;
    conda config --set always_yes yes --set changeps1 no;
    conda update -q conda;
    conda info -a;

    conda create -q -n test-environment python="${PYTHON_VERSION}" pip cython sympy nose pytest;
    source activate test-environment;
fi
if [[ "${CC}" == "" ]]; then
    export CC=gcc
    export CXX=g++
fi
