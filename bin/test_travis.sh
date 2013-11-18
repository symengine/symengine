#!/usr/bin/env bash

# Exit on error
set -e
# Echo each command
set -x

if [[ "${WITH_CSYMPY_RCP}" == "yes" ]]; then
    ctest --output-on-failure
    if [[ "${WITH_PYTHON}" == "yes" ]]; then
        nosetests -v
    fi
fi
