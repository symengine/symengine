# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import array
import math
import sys

import pytest
import numpy as np

import symengine as se


def test_Lambdify():
    n = 7
    args = x, y, z = se.symbols('x y z')
    l = se.Lambdify(args, [x+y+z, x**2, (x-y)/z, x*y*z])
    assert np.allclose(l(range(n, n+len(args))),
                       [3*n+3, n**2, -1/(n+2), n*(n+1)*(n+2)])


def test_Lambdify_2dim_numpy():
    args = x, y = se.symbols('x y')
    exprs = np.array([[x+y, x*y],
                      [x/y, x**y]])
    l = se.Lambdify(args, exprs)
    A = l((5, 7))
    assert abs(A[0, 0] - 12) < 1e-15
    assert abs(A[0, 1] - 35) < 1e-15
    assert abs(A[1, 0] - 5/7) < 1e-15
    assert abs(A[1, 1] - 5**7) < 1e-13

def _get_array():
    X, Y, Z = inp = array.array('d', [1, 2, 3])
    args = x, y, z = se.symbols('x y z')
    exprs = [x+y+z, se.sin(x)*se.log(y)*se.exp(z)]
    ref = [X+Y+Z, math.sin(X)*math.log(Y)*math.exp(Z)]
    return args, exprs, inp, ref


def test_array():
    args, exprs, inp, ref = _get_array()
    lmb = se.Lambdify(args, exprs)
    out = lmb(inp)
    assert all([abs(x1-x2) < 1e-15 for x1, x2 in zip(ref, out)])


@pytest.mark.skipif(sys.version_info[0] < 3, reason='requires Py3')
def test_array_out():
    args, exprs, inp, ref = _get_array()
    lmb = se.Lambdify(args, exprs)
    out1 = array.array('d', [0, 0, 0])
    out2 = lmb(inp, out1)
    assert out1 is out2
    assert all([abs(x1-x2) < 1e-15 for x1, x2 in zip(ref, out1)])
