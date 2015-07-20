#!/bin/bash
if [[ "$1" == "--help" || "$1" == "-h" || "$1" == "" ]]; then
    echo "./gdb.sh [file-name you want to run with rspec]"
    exit 0
fi
gdb -ex=r --args ruby -S rspec $1
