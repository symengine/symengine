# -*- coding: utf-8 -*-
from __future__ import (absolute_import, division, print_function)

import array
import math
import sys

import pytest

try:
    import numpy as np
    HAVE_NUMPY = True
except ImportError:
    HAVE_NUMPY = False


import symengine as se


def allclose(vec1, vec2, rtol=1e-13, atol=1e-13):
    return all([abs(a-b) < (atol + rtol*a) for a, b in zip(vec1, vec2)])


def test_Lambdify():
    n = 7
    args = x, y, z = se.symbols('x y z')
    l = se.Lambdify(args, [x+y+z, x**2, (x-y)/z, x*y*z])
    assert allclose(l(range(n, n+len(args))),
                    [3*n+3, n**2, -1/(n+2), n*(n+1)*(n+2)])


def _get_2_to_2by2_numpy():
    args = x, y = se.symbols('x y')
    exprs = np.array([[x+y, x*y],
                      [x/y, x**y]])
    l = se.Lambdify(args, exprs)
    def check(A, inp):
        X, Y = inp
        assert abs(A[0, 0] - (X+Y)) < 1e-15
        assert abs(A[0, 1] - (X*Y)) < 1e-15
        assert abs(A[1, 0] - (X/Y)) < 1e-15
        assert abs(A[1, 1] - (X**Y)) < 1e-13
    return l, check

@pytest.mark.skipif(not HAVE_NUMPY, reason='requires numpy')
def test_Lambdify_2dim_numpy():
    lmb, check = _get_2_to_2by2_numpy()
    for inp in [(5, 7), np.array([5, 7]), [5.0, 7.0]]:
        A = lmb(inp)
        assert A.shape == (2, 2)
        check(A, inp)


def _get_array():
    X, Y, Z = inp = array.array('d', [1, 2, 3])
    args = x, y, z = se.symbols('x y z')
    exprs = [x+y+z, se.sin(x)*se.log(y)*se.exp(z)]
    ref = [X+Y+Z, math.sin(X)*math.log(Y)*math.exp(Z)]

    def check(arr):
        assert all([abs(x1-x2) < 1e-15 for x1, x2 in zip(ref, arr)])
    return args, exprs, inp, check


def test_array():
    args, exprs, inp, check = _get_array()
    lmb = se.Lambdify(args, exprs)
    out = lmb(inp)
    check(out)


@pytest.mark.skipif(sys.version_info[0] < 3 or not HAVE_NUMPY,
                    reason='requires Py3 and NumPy')
def test_array_out():
    args, exprs, inp, check = _get_array()
    lmb = se.Lambdify(args, exprs)
    out1 = array.array('d', [0]*len(exprs))
    out2 = lmb(inp, out1)
    # Ensure buffer points to still data point:
    assert out1.buffer_info()[0] == out2.__array_interface__['data'][0]
    check(out1)
    check(out2)
    out2[:] = -1
    assert np.allclose(out1[:], [-1]*len(exprs))


@pytest.mark.skipif(sys.version_info[0] < 3, reason='requires Py3')
def test_array_out_no_numpy():
    args, exprs, inp, check = _get_array()
    lmb = se.Lambdify(args, exprs)
    out1 = array.array('d', [0]*len(exprs))
    out2 = lmb(inp, out1, use_numpy=False)
    # Ensure buffer points to still data point:
    assert out1.buffer_info() == out2.buffer_info()
    assert out1 is out2
    check(out1)
    check(out2)
    assert out2[0] != -1
    out1[0] = -1
    assert out2[0] == -1


def test_memview_out():
    args, exprs, inp, check = _get_array()
    lmb = se.Lambdify(args, exprs)
    cy_arr1 = lmb(inp, use_numpy=False)
    check(cy_arr1)
    cy_arr2 = lmb(inp, cy_arr1, use_numpy=False)
    check(cy_arr2)
    assert cy_arr2[0] != -1
    cy_arr1[0] = -1
    assert cy_arr2[0] == -1


@pytest.mark.skipif(not HAVE_NUMPY, reason='requires numpy')
def test_broadcast():
    a = np.linspace(-np.pi, np.pi)
    inp = np.vstack((np.cos(a), np.sin(a))).T  # 50 rows 2 cols
    x, y = se.symbols('x y')
    distance = se.Lambdify([x, y], [se.sqrt(x**2 + y**2)])
    assert np.allclose(distance([inp[0, 0], inp[0, 1]]), [1])
    dists = distance(inp)
    assert dists.shape == (50, 1)
    assert np.allclose(dists, 1)


def test_real_in_complex_out():
    x = se.Symbol('x')
    lmb = se.Lambdify([x], [se.sqrt(x)])
    assert abs(lmb([9]) - 3.0) < 1e-15
    assert abs(lmb([-9], complex_out=True) - 3.0j) < 1e-15


@pytest.mark.xfail
def test_complex_in_real_out():
    x = se.Symbol('x')
    lmb = se.Lambdify([x], [x*x])
    assert abs(lmb([3]) - 9) < 1e-15
    # The line below emits:
    # Exception RuntimeError: 'Not implemented.' in
    # 'symengine.lib.symengine_wrapper.as_real' ignored
    assert abs(lmb([3j], complex_in=True) + 9) < 1e-15


def test_complex_in_real_out2():
    x = se.Symbol('x')
    lmb = se.Lambdify([x], [x*x])
    assert abs(lmb([3j], complex_in=True, complex_out=True) + 9) < 3e-15


def test_complex_in_complex_out():
    x = se.Symbol('x')
    lmb = se.Lambdify([x], [3 + x - 1j])
    assert abs(lmb([11+13j], complex_in=True, complex_out=True) -
               (14 + 12j)) < 1e-15


@pytest.mark.skipif(not HAVE_NUMPY, reason='requires numpy')
def test_cse():
    # More of a demo than a test. This might be non-trivial enough
    # to warrant a helper function in Lambdify's __init__
    import sympy as sp
    inp = [11, 13]
    ref = [121+13, 13/121, 13*121 + 11]
    args = x, y = sp.symbols('x y')
    exprs = [x*x + y, y/(x*x), y*x*x+x]
    subs, new_exprs = sp.cse(exprs)
    cse_symbs, cse_exprs = zip(*subs)
    lmb = se.Lambdify(args + cse_symbs, new_exprs)
    cse_lambda = se.Lambdify(args, cse_exprs)
    cse_vals = cse_lambda(inp)
    new_inp = np.concatenate((inp, cse_vals))
    out = lmb(new_inp)
    assert allclose(out, ref)


def test_broadcast_c():
    n = 3
    inp = np.arange(2*n).reshape((n, 2))
    lmb, check = _get_2_to_2by2_numpy()
    A = lmb(inp)
    assert A.shape == (3, 2, 2)
    for i in range(n):
        check(A[i, ...], inp[i, :])


def test_broadcast_fortran():
    n = 3
    inp = np.arange(2*n).reshape((n, 2), order='F')
    lmb, check = _get_2_to_2by2_numpy()
    A = lmb(inp)
    assert A.shape == (3, 2, 2)
    for i in range(n):
        check(A[i, ...], inp[i, :])


def _get_1_to_2by3_matrix():
    x = se.symbols('x')
    args = x,
    exprs = se.DenseMatrix(2, 3, [x+1, x+2, x+3,
                                  1/x, 1/(x*x), 1/(x**3.0)])
    l = se.Lambdify(args, exprs)
    def check(A, inp):
        X, = inp
        assert abs(A[0, 0] - (X+1)) < 1e-15
        assert abs(A[0, 1] - (X+2)) < 1e-15
        assert abs(A[0, 2] - (X+3)) < 1e-15
        assert abs(A[1, 0] - (1/X)) < 1e-15
        assert abs(A[1, 1] - (1/(X*X))) < 1e-15
        assert abs(A[1, 2] - (1/(X**3.0))) < 1e-15
    return l, check


def test_2dim_Matrix():
    l, check = _get_1_to_2by3_matrix()
    inp = [7]
    check(l(inp), inp)
