#!/bin/bash

set -ex

re2c -W -g tok2.re -o tok2.cpp
#g++ -I../.. tok2.cpp
#./a.out a.txt
#flex tok.l
