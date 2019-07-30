#!/bin/bash

set -ex

flex -o tok.cpp --header-file=tok.h tok.l
