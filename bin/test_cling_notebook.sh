#!/bin/bash

unset CC
unset CXX
binder/postBuild
jupyter nbconvert --execute --to notebook notebooks/symengine.ipynb
