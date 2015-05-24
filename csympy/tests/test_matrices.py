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
    B = densematrix(2, 2, [-3, 2, 2, -1])

    assert A.inv('LU') == B
    assert A.inv('FFLU') == B
    assert A.inv('GJ') == B

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

def test_add_scalar():
    A = densematrix(2, 2, [1, 2, 3, 4])

    a = Symbol("a")
    assert A.add_scalar(a) == densematrix(2, 2, [1 + a, 2 + a, 3 + a, 4 + a])

    i5 = Integer(5)
    assert A.add_scalar(i5) == densematrix(2, 2, [6, 7, 8, 9])

def test_mul_scalar():
    A = densematrix(2, 2, [1, 2, 3, 4])

    a = Symbol("a")
    assert A.mul_scalar(a) == densematrix(2, 2, [a, 2*a, 3*a, 4*a])

    i5 = Integer(5)
    assert A.mul_scalar(i5) == densematrix(2, 2, [5, 10, 15, 20])

def test_transpose():
    A = densematrix(3, 3, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    assert A.transpose() == densematrix(3, 3, [1, 4, 7, 2, 5, 8, 3, 6, 9])

    A = densematrix(2, 2, [1, 2, 2, 1])

    assert A.transpose() == A

def test_submatrix():
    A = densematrix(3, 3, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    assert A.submatrix(0, 0, 0, 2) == densematrix(1, 3, [1, 2, 3])
    assert A.submatrix(2, 2, 0, 2) == densematrix(1, 3, [7, 8, 9])
    assert A.submatrix(0, 1, 1, 2) == densematrix(2, 2, [2, 3, 5, 6])
    assert A.submatrix(1, 2, 0, 2) == densematrix(2, 3, [4, 5, 6, 7, 8, 9])

def test_LU():
    A = densematrix(3, 3, [1, 3, 5, 2, 5, 6, 8, 3, 1]);
    L, U = A.LU()

    assert L == densematrix(3, 3, [1, 0, 0, 2, 1, 0, 8, 21, 1]);
    assert U == densematrix(3, 3, [1, 3, 5, 0, -1, -4, 0, 0, 45]);

def test_LDL():
    A = densematrix(3, 3, [4, 12, -16, 12, 37, -43, -16, -43, 98])

    L, D = A.LDL();

    assert L == densematrix(3, 3, [1, 0, 0, 3, 1, 0, -4, 5, 1])
    assert D == densematrix(3, 3, [4, 0, 0, 0, 1, 0, 0, 0, 9])

def test_solve():
    A = densematrix(4, 4, [1, 2, 3, 4, 2, 2, 3, 4, 3, 3, 3, 4, 9, 8, 7, 6])
    b = densematrix(4, 1, [10, 11, 13, 30])
    y = densematrix(4, 1, [1, 1, 1, 1]);

    x = A.solve(b, 'LU')
    assert x == y
    x = A.solve(b, 'FFLU')
    assert x == y
    x = A.solve(b, 'FFGJ')
    assert x == y

def test_FFLU():
    A = densematrix(4, 4, [1, 2, 3, 4, 2, 2, 3, 4, 3, 3, 3, 4, 9, 8, 7, 6])

    L, U = A.FFLU()
    assert L == densematrix(4, 4, [1, 0, 0, 0, 2, -2, 0, -0, 3, -3, 3, 0, 9, -10, 10, -10])
    assert U == densematrix(4, 4, [1, 2, 3, 4, 0, -2, -3, -4, 0, 0, 3, 4, 0, 0, 0, -10])

def test_FFLDU():
    A = densematrix(3, 3, [1, 2, 3, 5, -3, 2, 6, 2, 1])
    L, D, U = A.FFLDU()

    assert L == densematrix(3, 3, [1, 0, 0, 5, -13, 0, 6, -10, 1])
    assert D == densematrix(3, 3, [1, 0, 0, 0, -13, 0, 0, 0, -13])
    assert U == densematrix(3, 3, [1, 2, 3, 0, -13, -13, 0, 0, 91])
