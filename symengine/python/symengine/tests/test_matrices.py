from symengine import symbols
from symengine.lib.symengine_wrapper import (DenseMatrix, Symbol, Integer,
    function_symbol, I, NonSquareMatrixError)
from symengine.utilities import raises

def test_get():
    A = DenseMatrix(2, 2, [1, 2, 3, 4])

    assert A.get(0, 0) == 1
    assert A.get(0, 1) == 2
    assert A.get(1, 1) == 4

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = DenseMatrix(2, 2, [a, b, c, d])

    assert A.get(0, 0) == a
    assert A.get(1, 0) == c
    assert A.get(1, 1) == d

    assert A.get(-1, 0) == c
    assert A.get(-1, -1) == d

    raises(IndexError, lambda: A.get(2, 0))
    raises(IndexError, lambda: A.get(0, 2))
    raises(IndexError, lambda: A.get(-3, 0))

def test_get_item():
    A = DenseMatrix(3, 3, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    assert A[5] == 6
    assert A[-1] == 9
    assert A[2, 2] == 9
    assert A[-2, 2] == 6

    assert A[1:2, 0] == DenseMatrix(1, 1, [4])
    assert A[1:3, 0] == DenseMatrix(2, 1, [4, 7])
    assert A[1:3, 0] == DenseMatrix(2, 1, [4, 7])
    assert A[1:3, 1:] == DenseMatrix(2, 2, [5, 6, 8, 9])
    assert A[1:3, :1] == DenseMatrix(2, 1, [4, 7])
    assert A[1:] == DenseMatrix(2, 3, [4, 5, 6, 7, 8, 9])
    assert A[-2:] == DenseMatrix(2, 3, [4, 5, 6, 7, 8, 9])

    raises(IndexError, lambda: A[-10])
    raises(IndexError, lambda: A[9])

    raises(IndexError, lambda: A[1:3, 3])
    raises(IndexError, lambda: A[1:3, -4])

def test_set_item():
    A = DenseMatrix(3, 3, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    A[2] = 7
    A[2, 2] = 8
    A[-2] = 3
    A[-2, -1] = 1

    B = DenseMatrix(3, 3, [1, 2, 7, 4, 5, 1, 7, 3, 8])
    assert A == B

def test_set():
    i7 = Integer(7)
    y = Symbol("y")
    g = function_symbol("g", y)
    c = 2*I + 3
    A = DenseMatrix(2, 2,
        [Integer(5), Symbol("x"), function_symbol("f", Symbol("x")), 1 + I])

    A.set(0, 0, i7)
    assert A.get(0, 0) == i7
    A.set(0, 1, y)
    assert A.get(0, 1) == y
    A.set(1, 0, g)
    assert A.get(1, 0) == g
    A.set(1, 1, c)
    assert A.get(1, 1) == c

def test_det():
    A = DenseMatrix(2, 2, [1, 2, 3, 4])
    assert A.det() == -2

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = DenseMatrix(2, 2, [a, b, c, d])
    assert A.det() == a*d - b*c

    A = DenseMatrix(3, 2, [1, 2, 3, 4, 5, 6])
    raises(NonSquareMatrixError, lambda: A.det())

def test_inv():
    A = DenseMatrix(2, 2, [1, 0, 0, 1])
    assert A.inv() == A

    A = DenseMatrix(2, 2, [1, 2, 2, 3])
    B = DenseMatrix(2, 2, [-3, 2, 2, -1])

    assert A.inv('LU') == B
    assert A.inv('FFLU') == B
    assert A.inv('GJ') == B

def test_add_matrix():
    A = DenseMatrix(2, 2, [1, 2, 3, 4])
    B = DenseMatrix(2, 2, [1, 0, 0, 1])

    assert A.add_matrix(B) == DenseMatrix(2, 2, [2, 2, 3, 5])

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = DenseMatrix(2, 2, [a + b, a - b, a, b])
    B = DenseMatrix(2, 2, [a - b, a + b, -a, b])

    assert A.add_matrix(B) == DenseMatrix(2, 2, [2*a, 2*a, 0, 2*b])
    assert A + B == DenseMatrix(2, 2, [2*a, 2*a, 0, 2*b])

def test_mul_matrix():
    A = DenseMatrix(2, 2, [1, 2, 3, 4])
    B = DenseMatrix(2, 2, [1, 0, 0, 1])

    assert A.mul_matrix(B) == A

    a = Symbol("a")
    b = Symbol("b")
    c = Symbol("c")
    d = Symbol("d")
    A = DenseMatrix(2, 2, [a, b, c, d])
    B = DenseMatrix(2, 2, [1, 0, 1, 0])

    assert A.mul_matrix(B) == DenseMatrix(2, 2, [a + b, 0, c + d, 0])
    assert A * B == DenseMatrix(2, 2, [a + b, 0, c + d, 0])

    A = DenseMatrix(2, 3, [1, 2, 3, 2, 3, 4])
    B = DenseMatrix(3, 2, [3, 4, 4, 5, 5, 6])

    assert A.mul_matrix(B) == DenseMatrix(2, 2, [26, 32, 38, 47])

def test_add_scalar():
    A = DenseMatrix(2, 2, [1, 2, 3, 4])

    a = Symbol("a")
    assert A.add_scalar(a) == DenseMatrix(2, 2, [1 + a, 2 + a, 3 + a, 4 + a])

    i5 = Integer(5)
    assert A.add_scalar(i5) == DenseMatrix(2, 2, [6, 7, 8, 9])
    assert A + 5 == DenseMatrix(2, 2, [6, 7, 8, 9])
    assert 5 + A == DenseMatrix(2, 2, [6, 7, 8, 9])

def test_mul_scalar():
    A = DenseMatrix(2, 2, [1, 2, 3, 4])

    a = Symbol("a")
    assert A.mul_scalar(a) == DenseMatrix(2, 2, [a, 2*a, 3*a, 4*a])

    i5 = Integer(5)
    assert A.mul_scalar(i5) == DenseMatrix(2, 2, [5, 10, 15, 20])
    assert A * 5 == DenseMatrix(2, 2, [5, 10, 15, 20])
    assert 5 * A == DenseMatrix(2, 2, [5, 10, 15, 20])

def test_transpose():
    A = DenseMatrix(3, 3, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    assert A.transpose() == DenseMatrix(3, 3, [1, 4, 7, 2, 5, 8, 3, 6, 9])

    A = DenseMatrix(2, 2, [1, 2, 2, 1])

    assert A.transpose() == A

def test_submatrix():
    A = DenseMatrix(3, 3, [1, 2, 3, 4, 5, 6, 7, 8, 9])

    assert A.submatrix(0, 0, 0, 2) == DenseMatrix(1, 3, [1, 2, 3])
    assert A.submatrix(2, 2, 0, 2) == DenseMatrix(1, 3, [7, 8, 9])
    assert A.submatrix(0, 1, 1, 2) == DenseMatrix(2, 2, [2, 3, 5, 6])
    assert A.submatrix(1, 2, 0, 2) == DenseMatrix(2, 3, [4, 5, 6, 7, 8, 9])

def test_LU():
    A = DenseMatrix(3, 3, [1, 3, 5, 2, 5, 6, 8, 3, 1]);
    L, U = A.LU()

    assert L == DenseMatrix(3, 3, [1, 0, 0, 2, 1, 0, 8, 21, 1]);
    assert U == DenseMatrix(3, 3, [1, 3, 5, 0, -1, -4, 0, 0, 45]);

def test_LDL():
    A = DenseMatrix(3, 3, [4, 12, -16, 12, 37, -43, -16, -43, 98])

    L, D = A.LDL();

    assert L == DenseMatrix(3, 3, [1, 0, 0, 3, 1, 0, -4, 5, 1])
    assert D == DenseMatrix(3, 3, [4, 0, 0, 0, 1, 0, 0, 0, 9])

def test_solve():
    A = DenseMatrix(4, 4, [1, 2, 3, 4, 2, 2, 3, 4, 3, 3, 3, 4, 9, 8, 7, 6])
    b = DenseMatrix(4, 1, [10, 11, 13, 30])
    y = DenseMatrix(4, 1, [1, 1, 1, 1]);

    x = A.solve(b, 'LU')
    assert x == y
    x = A.solve(b, 'FFLU')
    assert x == y
    x = A.solve(b, 'FFGJ')
    assert x == y

def test_FFLU():
    A = DenseMatrix(4, 4, [1, 2, 3, 4, 2, 2, 3, 4, 3, 3, 3, 4, 9, 8, 7, 6])

    L, U = A.FFLU()
    assert L == DenseMatrix(4, 4, [1, 0, 0, 0, 2, -2, 0, -0, 3, -3, 3, 0, 9, -10, 10, -10])
    assert U == DenseMatrix(4, 4, [1, 2, 3, 4, 0, -2, -3, -4, 0, 0, 3, 4, 0, 0, 0, -10])

def test_FFLDU():
    A = DenseMatrix(3, 3, [1, 2, 3, 5, -3, 2, 6, 2, 1])
    L, D, U = A.FFLDU()

    assert L == DenseMatrix(3, 3, [1, 0, 0, 5, -13, 0, 6, -10, 1])
    assert D == DenseMatrix(3, 3, [1, 0, 0, 0, -13, 0, 0, 0, -13])
    assert U == DenseMatrix(3, 3, [1, 2, 3, 0, -13, -13, 0, 0, 91])

def test_str_repr():
    d = DenseMatrix(3, 2, [1, 2, 3, 4, 5, 6])
    assert str(d) == '[1, 2]\n[3, 4]\n[5, 6]\n'
    assert str(d) == repr(d)

def test_DenseMatrix_symbols():
    x, y, z = symbols("x y z")
    D = DenseMatrix(4, 4,
            [1, 0, 1, 0,
            0, z, y, 0,
            z, 1, x, 1,
            1, 1, 0, 0])
    assert D.get(1, 2) == y

def test_jacobian():
    x, y, z, t = symbols("x y z t")
    J_correct = DenseMatrix(4, 4,
            [1, 0, 1, 0,
            0, z, y, 0,
            z, 1, x, 1,
            1, 1, 0, 0])
    D = DenseMatrix(4, 1, [x+z, y*z, z*x+y+t, x+y])
    x = DenseMatrix(4, 1, [x, y, z, t])
    J = D.jacobian(x)
    assert J == J_correct
