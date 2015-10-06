# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import symengine as se
import numpy as np
from symengine import Lambdify

def test_Lambdify():
    n = 7
    args = x, y, z = se.symbols('x y z')
    l = Lambdify(args, [x+y+z, x**2, (x-y)/z, x*y*z])
    assert np.allclose(l(range(n, n+len(args))),
                       [3*n+3, n**2, -1/(n+2), n*(n+1)*(n+2)])

def test_Lambdify_2dim_numpy():
    args = x, y = se.symbols('x y')
    exprs = np.array([[x+y, x*y],
                      [x/y, x**y]])
    l = Lambdify(args, exprs)
    A = l((5, 7))
    assert abs(A[0, 0] - 12) < 1e-15
    assert abs(A[0, 1] - 35) < 1e-15
    assert abs(A[1, 0] - 5/7) < 1e-15
    assert abs(A[1, 1] - 5**7) < 1e-13
