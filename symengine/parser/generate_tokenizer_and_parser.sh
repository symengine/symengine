#!/usr/bin/env bash

# commands to generate the files tokenizer.cpp, parser.tab.cc, parser.tab.hh
# requires re2c and bison to be installed

re2c tokenizer.re -s -b --no-generation-date -o tokenizer.cpp
bison parser.yy -d -o parser.tab.cc
