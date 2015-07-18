#!/bin/bash

valgrind --tool=memcheck --num-callers=15 --partial-loads-ok=yes --undef-value-errors=no ruby -S rspec spec
