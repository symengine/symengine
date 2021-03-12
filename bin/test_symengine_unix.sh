#!/bin/bash

if [[ "$(uname)" == "Linux" ]]; then
  sudo apt update
  sudo apt install software-properties-common
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test
  if [[ "$EXTRA_APT_REPOSITORY" != "" ]]; then
      sudo add-apt-repository "$EXTRA_APT_REPOSITORY"
  fi
  sudo apt update
  sudo apt install binutils-dev g++-4.7 $EXTRA_APT_PACKAGES
fi

if [[ "$TEST_CLANG_FORMAT" == "yes" ]]; then
  source bin/travis_clang_format.sh
elif [[ "$CONDA_ENV_FILE" == *"matchpycpp"* ]]; then
  source bin/test_matchpycpp_gen_tests.sh
else
  source bin/install_travis.sh
  source bin/test_travis.sh
fi
