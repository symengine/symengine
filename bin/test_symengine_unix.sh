#!/bin/bash

if [[ "$(uname)" == "Linux" ]]; then
  sudo apt update
  sudo apt install software-properties-common
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test
  if [[ "$EXTRA_APT_REPOSITORY" != "" ]]; then
      sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 15CF4D18AF4F7421
      sudo add-apt-repository "$EXTRA_APT_REPOSITORY"
  fi
  sudo apt update
  sudo apt install binutils-dev g++ $EXTRA_APT_PACKAGES
fi

if [[ "$TEST_CLANG_FORMAT" == "yes" ]]; then
  source bin/travis_clang_format.sh
elif [[ "$CONDA_ENV_FILE" == *"matchpycpp"* ]]; then
  source bin/install_travis.sh
  source bin/test_matchpycpp_gen_tests.sh
elif [[ "$MSYSTEM" != "" ]]; then
  export SOURCE_DIR=`pwd`
  export our_install_dir="$HOME/our_usr"
  export CMAKE_GENERATOR="Unix Makefiles"
  export CXX="ccache g++"
  export CC="ccache gcc"
  export CCACHE_DIR=$HOME/.ccache
  ccache -M 100M
  ccache --version
  ccache --zero-stats
  ccache --show-stats
  source bin/test_travis.sh
else
  source bin/install_travis.sh
  source bin/test_travis.sh
fi
