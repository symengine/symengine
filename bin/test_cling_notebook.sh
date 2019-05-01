#!/bin/bash

export CONDA_DIR=$CONDA_PREFIX
binder/postBuild
jupyter nbconvert --execute --to notebook notebooks/symengine.ipynb
