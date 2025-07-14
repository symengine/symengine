#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

if [[ "${RUNNER_OS}" == "Linux" ]]; then
  sudo apt update
  sudo apt install software-properties-common
  if ! grep grep DISTRIB_CODENAME=jammy /etc/lsb-release >/dev/null; then
      sudo add-apt-repository ppa:ubuntu-toolchain-r/test
  fi
  if [[ "$EXTRA_APT_REPOSITORY" != "" ]]; then
      sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 15CF4D18AF4F7421
      sudo add-apt-repository "$EXTRA_APT_REPOSITORY"
  fi
  sudo apt update
  if [[ "$WITH_LATEST_GCC" != "yes" ]]; then
      EXTRA_APT_PACKAGES="$EXTRA_APT_PACKAGES g++"
  fi
  sudo apt install binutils-dev $EXTRA_APT_PACKAGES
fi

if [[ "${CC}" == "" ]]; then
    if [[ "${RUNNER_OS}" == "macOS" ]]; then
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

if [[ "${RUNNER_OS}" == "Linux" ]] && [[ "${CC}" == "gcc" ]]; then
    if [[ "${WITH_LATEST_GCC}" == "yes" ]]; then
        export CC=gcc-12
        export CXX=g++-12
        export GCOV_EXECUTABLE=gcov-12
    else
        if grep DISTRIB_CODENAME=jammy /etc/lsb-release >/dev/null; then
            export CC=gcc-11
            export CXX=g++-11
            export GCOV_EXECUTABLE=gcov-11
        else
            export CC=gcc-9
            export CXX=g++-9
            export GCOV_EXECUTABLE=gcov-9
        fi
    fi
fi

export SOURCE_DIR=`pwd`
if [[ "${MSYS_ENV}" != "" ]]; then
  export our_install_dir="$HOME/our_usr"
else
  export our_install_dir="${CONDA_PREFIX}"
  if [[ "$MSYSTEM" != "" ]]; then
    # If we are using git bash on Windows, ensure the path is in Unix format:
    our_install_dir=$(cygpath -u "${CONDA_PREFIX}")/Library
  fi

  # Useful for debugging any issues with conda
  conda info -a

  conda_pkgs="$conda_pkgs ccache"
  if [[ "${INTEGER_CLASS}" == "boostmp" ]]; then
      conda_pkgs="$conda_pkgs boost>=1.80.0"
      if [[ "$MSYSTEM" == "" ]]; then
        export CXXFLAGS="-Wno-error=cpp"  # boost-1.80 pragma-warns about 1.82 req. c++14
      fi
  else
      conda_pkgs="$conda_pkgs gmp=6.3.0"
  fi

  if [[ "${WITH_BENCHMARKS_GOOGLE}" == "yes" ]]; then
      conda_pkgs="${conda_pkgs} benchmark=1.7.1"
  fi

  if [[ "${WITH_PIRANHA}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs piranha=0.11 cmake=3.24.3 boost-cpp=1.78.0"
  fi

  if [[ "${WITH_PRIMESIEVE}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs primesieve=8.0"
  fi

  if [[ "${WITH_MPFR}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs mpfr=4.2.1"
  fi

  if [[ "${WITH_MPC}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs mpc=1.3.1"
  fi

  if [[ "${WITH_FLINT}" == "yes" ]] && [[ "${WITH_FLINT_DEV}" != "yes" ]]; then
      conda_pkgs="$conda_pkgs libflint=3.2.2"
  fi

  if [[ "${WITH_ARB}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs arb=2.23.0"
  fi

  if [[ "${WITH_LLVM}" == "LATEST" ]]; then
      WITH_LLVM=$(ls -d /usr/lib/llvm-?? | awk -F- '{print $2}' | sort -g | tail -n 1)
  fi

  if [[ ! -z "${WITH_LLVM}" ]]; then
      if [[ "${EXTRA_APT_PACKAGES}" == *"llvm"* ]]; then
          export LLVM_DIR="/usr/lib/llvm-${WITH_LLVM}/share/llvm/"
      else
          conda_pkgs="$conda_pkgs llvmdev=${WITH_LLVM} cmake=3.24.3"
          export LLVM_DIR=$our_install_dir/share/llvm/
      fi
  fi

  if [[ "${WITH_ECM}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs ecm=7.0.4"
  fi

  if [[ "${BUILD_DOXYGEN}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs doxygen=1.9.5"
  fi

  if [[ "${BUILD_TUTORIALS}" == "yes" ]]; then
      conda_pkgs="$conda_pkgs jupytext papermill xeus-cling"
  fi

  retry_on_error () {
    "$@" || (sleep 5 && "$@") || (sleep 30 && "$@") || (sleep 120 && "$@")
  }

  retry_on_error conda install -q ${conda_pkgs};

fi

export CXX="ccache ${CXX}"
export CC="ccache ${CC}"
export CCACHE_SLOPPINESS="pch_defines,time_macros"
ccache -M 100M
ccache --version
ccache --zero-stats
ccache --show-stats

if [[ "${WITH_FLINT_DEV}" == "yes" ]] && [[ "${WITH_ARB}" != "yes" ]]; then
    git clone https://github.com/flintlib/flint2;
    cd flint2 && ./bootstrap.sh && ./configure --prefix=$our_install_dir --with-gmp=$our_install_dir --with-mpfr=$our_install_dir && make -j4 install && cd ..;
fi

cd $SOURCE_DIR;

# Since this script is getting sourced, remove error on exit
set +e
set +x
