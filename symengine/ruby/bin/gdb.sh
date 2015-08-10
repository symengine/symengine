#!/bin/bash

# Note: use this script from the root dir of the ruby wrappers gem as
# your current dir.

if [[ "$1" == "--help" || "$1" == "-h" || "$1" == "" ]]; then
    echo "bin/gdb.sh [file-name you want to run with rspec]"
    exit 0
fi
gdb -ex=r --args ruby -S rspec $1
