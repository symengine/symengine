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
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DWITH_BFD=${WITH_BFD} -DWITH_PYTHON=${WITH_PYTHON} -DWITH_CSYMPY_ASSERT=${WITH_CSYMPY_ASSERT} -DWITH_CSYMPY_RCP=${WITH_CSYMPY_RCP} -DWITH_ECM=${WITH_ECM} ${SOURCE_DIR}
echo "Current directory:"
pwd
echo "Running make:"
make

if [[ "${WITH_CSYMPY_RCP}" == "yes" ]]; then
    ctest --output-on-failure
    if [[ "${WITH_PYTHON}" == "yes" ]]; then
        nosetests -v
    fi
fi
