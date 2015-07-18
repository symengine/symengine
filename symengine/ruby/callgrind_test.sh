#!/bin/bash

valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ruby -S rspec spec
