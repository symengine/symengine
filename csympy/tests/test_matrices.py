from nose.tools import raises
from csympy.lib.csympy_wrapper import (densematrix, Symbol, Integer,
    function_symbol, I)

def test_get():
    A = densematrix(2, 2, [1, 2, 3, 4])

    assert A.get(0, 0) == 1
    assert A.get(0, 1) == 2
    assert A.get(1, 1) == 4

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = densematrix(2, 2, [a, b, c, d])

    assert A.get(0, 0) == a
    assert A.get(1, 0) == c
    assert A.get(1, 1) == d

def test_set():
    i7 = Integer(7)
    y = Symbol("y")
    g = function_symbol("g", y)
    c = 2*I + 3
    A = densematrix(2, 2,
        [Integer(5), Symbol("x"), function_symbol("f", Symbol("x")), 1 + I])

    A.set(0, 0, i7)
    assert A.get(0, 0) == i7
    A.set(0, 1, y)
    assert A.get(0, 1) == y
    A.set(1, 0, g)
    assert A.get(1, 0) == g
    A.set(1, 1, c)
    A.get(1, 1) == c

def test_det():
    A = densematrix(2, 2, [1, 2, 3, 4])
    assert A.det() == -2

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = densematrix(2, 2, [a, b, c, d])
    assert A.det() == a*d - b*c

def test_inv():
    A = densematrix(2, 2, [1, 0, 0, 1])
    assert A.inv() == A

    A = densematrix(2, 2, [1, 2, 2, 3])
    assert A.inv() == densematrix(2, 2, [-3, 2, 2, -1])

def test_add_matrix():
    A = densematrix(2, 2, [1, 2, 3, 4])
    B = densematrix(2, 2, [1, 0, 0, 1])

    assert A.add_matrix(B) == densematrix(2, 2, [2, 2, 3, 5])

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = densematrix(2, 2, [a + b, a - b, a, b])
    B = densematrix(2, 2, [a - b, a + b, -a, b])

    assert A.add_matrix(B) == densematrix(2, 2, [2*a, 2*a, 0, 2*b])

def test_mul_matrix():
    A = densematrix(2, 2, [1, 2, 3, 4])
    B = densematrix(2, 2, [1, 0, 0, 1])

    assert A.mul_matrix(B) == A

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = densematrix(2, 2, [a, b, c, d])
    B = densematrix(2, 2, [1, 0, 1, 0])

    assert A.mul_matrix(B) == densematrix(2, 2, [a + b, 0, c + d, 0])
