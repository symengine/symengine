#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

if [[ "${TEST_IN_TREE}" == "yes" ]]; then
    export SOURCE_DIR=.
else
    mkdir build
    cd build
    export SOURCE_DIR=..
fi
echo "Current directory:"
pwd
echo "Running cmake:"
# We build the command line here. If the variable is empty, we skip it,
# otherwise we pass it to cmake.
cmake_line=""
if [[ "${BUILD_TYPE}" != "" ]]; then
    cmake_line="$cmake_line -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
fi
if [[ "${WITH_BFD}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_BFD=${WITH_BFD}"
fi
if [[ "${WITH_PYTHON}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_PYTHON=${WITH_PYTHON}"
fi
if [[ "${WITH_CSYMPY_ASSERT}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_CSYMPY_ASSERT=${WITH_CSYMPY_ASSERT}"
fi
if [[ "${WITH_CSYMPY_RCP}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_CSYMPY_RCP=${WITH_CSYMPY_RCP}"
fi
if [[ "${WITH_ECM}" != "" ]]; then
    cmake_line="$cmake_line -DWITH_ECM=${WITH_ECM}"
fi
cmake $cmake_line ${SOURCE_DIR}
echo "Current directory:"
pwd
echo "Running make:"
make

if [[ "${WITH_CSYMPY_RCP}" == "no" ]]; then
    echo "CSymPy successfully built with Teuchos::RCP. No tests being run."
else
    ctest --output-on-failure
    if [[ "${WITH_PYTHON}" == "yes" ]]; then
        nosetests -v
    fi
fi
