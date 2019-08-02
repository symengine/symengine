#!/bin/bash

set -ex

bison -L c++ -d parser.yy
