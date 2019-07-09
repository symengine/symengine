#!/bin/bash

export NB_PYTHON_PREFIX=$CONDA_PREFIX
binder/postBuild
jupyter nbconvert --execute --to notebook notebooks/symengine.ipynb
