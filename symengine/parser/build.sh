#!/bin/bash

set -ex

flex --c++ -o tok.cpp --header-file=tok.h tok.l
