#!/bin/bash

set -ex

re2c tok2.re -o tok2.cpp
flex tok.l
