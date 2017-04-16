#!/bin/sh
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_BUILD_TYPE=Release -DWITH_PYTHON=yes -DPYTHON_BIN=$PYTHON ..
make install
