from symengine import (Symbol, Integer, sympify, SympifyError, log,
        function_symbol, I, E, pi, exp, have_mpfr, have_mpc, DenseMatrix,
        sin, cos, tan, cot, csc, sec, asin, acos, atan, acot, acsc, asec,
        sinh, cosh, tanh, coth, asinh, acosh, atanh, acoth)
from symengine.lib.symengine_wrapper import Subs, Derivative, RealMPFR, ComplexMPC
import sympy

# Note: We test _sympy_() for SymEngine -> SymPy conversion, as those are methods
# that are implemented in this library. Users can simply use sympy.sympify(...)
# to do this conversion, as this function will call our _sympy_() methods under
# the hood.
#
# For SymPy -> SymEngine, we test symengine.sympify(...) which does the conversion.

def test_conv1():
    x = Symbol("x")
    assert x._sympy_() == sympy.Symbol("x")
    assert x._sympy_() != sympy.Symbol("y")
    x = Symbol("y")
    assert x._sympy_() != sympy.Symbol("x")
    assert x._sympy_() == sympy.Symbol("y")

def test_conv1b():
    x = sympy.Symbol("x")
    assert sympify(x) == Symbol("x")
    assert sympify(x) != Symbol("y")
    x = sympy.Symbol("y")
    assert sympify(x) != Symbol("x")
    assert sympify(x) == Symbol("y")

def test_conv2():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    e = x*y
    assert e._sympy_() == sympy.Symbol("x")*sympy.Symbol("y")
    e = x*y*z
    assert e._sympy_() == sympy.Symbol("x")*sympy.Symbol("y")*sympy.Symbol("z")

def test_conv2b():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    z = sympy.Symbol("z")
    e = x*y
    assert sympify(e) == Symbol("x")*Symbol("y")
    e = x*y*z
    assert sympify(e) == Symbol("x")*Symbol("y")*Symbol("z")

def test_conv3():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    e = x+y
    assert e._sympy_() == sympy.Symbol("x")+sympy.Symbol("y")
    e = x+y+z
    assert e._sympy_() == sympy.Symbol("x")+sympy.Symbol("y")+sympy.Symbol("z")

def test_conv3b():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    z = sympy.Symbol("z")
    e = x+y
    assert sympify(e) == Symbol("x")+Symbol("y")
    e = x+y+z
    assert sympify(e) == Symbol("x")+Symbol("y")+Symbol("z")

def test_conv4():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    e = x**y
    assert e._sympy_() == sympy.Symbol("x")**sympy.Symbol("y")
    e = (x+y)**z
    assert e._sympy_() == (sympy.Symbol("x")+sympy.Symbol("y"))**sympy.Symbol("z")

def test_conv4b():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    z = sympy.Symbol("z")
    e = x**y
    assert sympify(e) == Symbol("x")**Symbol("y")
    e = (x+y)**z
    assert sympify(e) == (Symbol("x")+Symbol("y"))**Symbol("z")

def test_conv5():
    x = Integer(5)
    y = Integer(6)
    assert x._sympy_() == sympy.Integer(5)
    assert (x/y)._sympy_() == sympy.Integer(5) / sympy.Integer(6)

def test_conv5b():
    x = sympy.Integer(5)
    y = sympy.Integer(6)
    assert sympify(x) == Integer(5)
    assert sympify(x/y) == Integer(5) / Integer(6)

def test_conv6():
    x = Symbol("x")
    y = Symbol("y")
    assert (x/3)._sympy_() == sympy.Symbol("x") / 3
    assert (3*x)._sympy_() == 3*sympy.Symbol("x")
    assert (3+x)._sympy_() == 3+sympy.Symbol("x")
    assert (3-x)._sympy_() == 3-sympy.Symbol("x")
    assert (x/y)._sympy_() == sympy.Symbol("x") / sympy.Symbol("y")

def test_conv6b():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    assert sympify(x/3) == Symbol("x") / 3
    assert sympify(3*x) == 3*Symbol("x")
    assert sympify(3+x) == 3+Symbol("x")
    assert sympify(3-x) == 3-Symbol("x")
    assert sympify(x/y) == Symbol("x") / Symbol("y")

def test_conv7():
    x = Symbol("x")
    y = Symbol("y")
    assert sin(x/3) == sin(sympy.Symbol("x") / 3)
    assert cos(x/3) == cos(sympy.Symbol("x") / 3)
    assert tan(x/3) == tan(sympy.Symbol("x") / 3)
    assert cot(x/3) == cot(sympy.Symbol("x") / 3)
    assert csc(x/3) == csc(sympy.Symbol("x") / 3)
    assert sec(x/3) == sec(sympy.Symbol("x") / 3)
    assert asin(x/3) == asin(sympy.Symbol("x") / 3)
    assert acos(x/3) == acos(sympy.Symbol("x") / 3)
    assert atan(x/3) == atan(sympy.Symbol("x") / 3)
    assert acot(x/3) == acot(sympy.Symbol("x") / 3)
    assert acsc(x/3) == acsc(sympy.Symbol("x") / 3)
    assert asec(x/3) == asec(sympy.Symbol("x") / 3)

    assert sin(x/3)._sympy_() == sympy.sin(sympy.Symbol("x") / 3)
    assert sin(x/3)._sympy_() != sympy.cos(sympy.Symbol("x") / 3)
    assert cos(x/3)._sympy_() == sympy.cos(sympy.Symbol("x") / 3)
    assert tan(x/3)._sympy_() == sympy.tan(sympy.Symbol("x") / 3)
    assert cot(x/3)._sympy_() == sympy.cot(sympy.Symbol("x") / 3)
    assert csc(x/3)._sympy_() == sympy.csc(sympy.Symbol("x") / 3)
    assert sec(x/3)._sympy_() == sympy.sec(sympy.Symbol("x") / 3)
    assert asin(x/3)._sympy_() == sympy.asin(sympy.Symbol("x") / 3)
    assert acos(x/3)._sympy_() == sympy.acos(sympy.Symbol("x") / 3)
    assert atan(x/3)._sympy_() == sympy.atan(sympy.Symbol("x") / 3)
    assert acot(x/3)._sympy_() == sympy.acot(sympy.Symbol("x") / 3)
    assert acsc(x/3)._sympy_() == sympy.acsc(sympy.Symbol("x") / 3)
    assert asec(x/3)._sympy_() == sympy.asec(sympy.Symbol("x") / 3)

def test_conv7b():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    assert sympify(sympy.sin(x/3)) == sin(Symbol("x") / 3)
    assert sympify(sympy.sin(x/3)) != cos(Symbol("x") / 3)
    assert sympify(sympy.cos(x/3)) == cos(Symbol("x") / 3)
    assert sympify(sympy.tan(x/3)) == tan(Symbol("x") / 3)
    assert sympify(sympy.cot(x/3)) == cot(Symbol("x") / 3)
    assert sympify(sympy.csc(x/3)) == csc(Symbol("x") / 3)
    assert sympify(sympy.sec(x/3)) == sec(Symbol("x") / 3)
    assert sympify(sympy.asin(x/3)) == asin(Symbol("x") / 3)
    assert sympify(sympy.acos(x/3)) == acos(Symbol("x") / 3)
    assert sympify(sympy.atan(x/3)) == atan(Symbol("x") / 3)
    assert sympify(sympy.acot(x/3)) == acot(Symbol("x") / 3)
    assert sympify(sympy.acsc(x/3)) == acsc(Symbol("x") / 3)
    assert sympify(sympy.asec(x/3)) == asec(Symbol("x") / 3)

def test_conv8():
    e1 = function_symbol("f", Symbol("x"))
    e2 = function_symbol("g", Symbol("x"), Symbol("y"))
    assert e1._sympy_() == sympy.Function("f")(sympy.Symbol("x"))
    assert e2._sympy_() != sympy.Function("f")(sympy.Symbol("x"))
    assert e2._sympy_() == sympy.Function("g")(sympy.Symbol("x"), sympy.Symbol("y"))

    e3 = function_symbol("q", Symbol("t"))
    assert e3._sympy_() == sympy.Function("q")(sympy.Symbol("t"))
    assert e3._sympy_() != sympy.Function("f")(sympy.Symbol("t"))
    assert e3._sympy_() != sympy.Function("q")(sympy.Symbol("t"), sympy.Symbol("t"))

def test_conv8b():
    e1 = sympy.Function("f")(sympy.Symbol("x"))
    e2 = sympy.Function("g")(sympy.Symbol("x"), sympy.Symbol("y"))
    assert sympify(e1) == function_symbol("f", Symbol("x"))
    assert sympify(e2) != function_symbol("f", Symbol("x"))
    assert sympify(e2) == function_symbol("g", Symbol("x"), Symbol("y"))

    e3 = sympy.Function("q")(sympy.Symbol("t"))
    assert sympify(e3) == function_symbol("q", Symbol("t"))
    assert sympify(e3) != function_symbol("f", Symbol("t"))
    assert sympify(e3) != function_symbol("q", Symbol("t"), Symbol("t"))

def test_conv9():
    x = Symbol("x")
    y = Symbol("y")
    assert (I)._sympy_() == sympy.I
    assert (2*I+3)._sympy_() == 2*sympy.I+3
    assert (2*I/5+Integer(3)/5)._sympy_() == 2*sympy.I/5+sympy.S(3)/5
    assert (x*I+3)._sympy_() == sympy.Symbol("x")*sympy.I + 3
    assert (x+I*y)._sympy_() == sympy.Symbol("x") + sympy.I*sympy.Symbol("y")

def test_conv9b():
    x = Symbol("x")
    y = Symbol("y")
    assert sympify(sympy.I) == I
    assert sympify(2*sympy.I+3) == 2*I+3
    assert sympify(2*sympy.I/5+sympy.S(3)/5) == 2*I/5+Integer(3)/5
    assert sympify(sympy.Symbol("x")*sympy.I + 3) == x*I+3
    assert sympify(sympy.Symbol("x") + sympy.I*sympy.Symbol("y")) == x+I*y

def test_conv10():
    A = DenseMatrix(1, 4, [Integer(1), Integer(2), Integer(3), Integer(4)])
    assert A._sympy_() == sympy.Matrix(1, 4, [sympy.Integer(1), sympy.Integer(2),
        sympy.Integer(3), sympy.Integer(4)])

    B = DenseMatrix(4, 1, [Symbol("x"), Symbol("y"), Symbol("z"), Symbol("t")])
    assert B._sympy_() == sympy.Matrix(4, 1, [sympy.Symbol("x"), sympy.Symbol("y"),
        sympy.Symbol("z"), sympy.Symbol("t")])

    C = DenseMatrix(2, 2,
        [Integer(5), Symbol("x"), function_symbol("f", Symbol("x")), 1 + I])

    assert C._sympy_() == sympy.Matrix([[5, sympy.Symbol("x")],
        [sympy.Function("f")(sympy.Symbol("x")), 1 + sympy.I]])

def test_conv10b():
    A = sympy.Matrix([[sympy.Symbol("x"), sympy.Symbol("y")],
        [sympy.Symbol("z"), sympy.Symbol("t")]])
    assert sympify(A) == DenseMatrix(2, 2, [Symbol("x"), Symbol("y"),
        Symbol("z"), Symbol("t")])

    B = sympy.Matrix([[1, 2], [3, 4]])
    assert sympify(B) == DenseMatrix(2, 2, [Integer(1), Integer(2), Integer(3),
        Integer(4)])

    C = sympy.Matrix([[7, sympy.Symbol("y")],
        [sympy.Function("g")(sympy.Symbol("z")), 3 + 2*sympy.I]])
    assert sympify(C) == DenseMatrix(2, 2, [Integer(7), Symbol("y"),
        function_symbol("g", Symbol("z")), 3 + 2*I])

def test_conv11():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    x1 = Symbol("x")
    y1 = Symbol("y")
    e1 = sympy.Subs(sympy.Derivative(sympy.Function("f")(x, y), x), [x, y], [y, y])

    e2 = Subs(Derivative(function_symbol("f", x1, y1), [x1]), [x1, y1], [y1, y1])
    e3 = Subs(Derivative(function_symbol("f", x1, y1), [x1]), [y1, x1], [x1, y1])

    assert sympify(e1) == e2
    assert sympify(e1) != e3

    assert e2._sympy_() == e1
    assert e3._sympy_() != e1

def test_conv12():
    x = Symbol("x")
    y = Symbol("y")
    assert sinh(x/3) == sinh(sympy.Symbol("x") / 3)
    assert cosh(x/3) == cosh(sympy.Symbol("x") / 3)
    assert tanh(x/3) == tanh(sympy.Symbol("x") / 3)
    assert coth(x/3) == coth(sympy.Symbol("x") / 3)
    assert asinh(x/3) == asinh(sympy.Symbol("x") / 3)
    assert acosh(x/3) == acosh(sympy.Symbol("x") / 3)
    assert atanh(x/3) == atanh(sympy.Symbol("x") / 3)
    assert acoth(x/3) == acoth(sympy.Symbol("x") / 3)

    assert sinh(x/3)._sympy_() == sympy.sinh(sympy.Symbol("x") / 3)
    assert cosh(x/3)._sympy_() == sympy.cosh(sympy.Symbol("x") / 3)
    assert tanh(x/3)._sympy_() == sympy.tanh(sympy.Symbol("x") / 3)
    assert coth(x/3)._sympy_() == sympy.coth(sympy.Symbol("x") / 3)
    assert asinh(x/3)._sympy_() == sympy.asinh(sympy.Symbol("x") / 3)
    assert acosh(x/3)._sympy_() == sympy.acosh(sympy.Symbol("x") / 3)
    assert atanh(x/3)._sympy_() == sympy.atanh(sympy.Symbol("x") / 3)
    assert acoth(x/3)._sympy_() == sympy.acoth(sympy.Symbol("x") / 3)

def test_conv12b():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    assert sympify(sympy.sinh(x/3)) == sinh(Symbol("x") / 3)
    assert sympify(sympy.cosh(x/3)) == cosh(Symbol("x") / 3)
    assert sympify(sympy.tanh(x/3)) == tanh(Symbol("x") / 3)
    assert sympify(sympy.coth(x/3)) == coth(Symbol("x") / 3)
    assert sympify(sympy.asinh(x/3)) == asinh(Symbol("x") / 3)
    assert sympify(sympy.acosh(x/3)) == acosh(Symbol("x") / 3)
    assert sympify(sympy.atanh(x/3)) == atanh(Symbol("x") / 3)
    assert sympify(sympy.acoth(x/3)) == acoth(Symbol("x") / 3)

def test_tuples_lists():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    z = sympy.Symbol("z")
    l = [x, y, z, x*y, z**y]
    t = (x, y, z, x*y, z**y)
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    l2 = [x, y, z, x*y, z**y]
    t2 = (x, y, z, x*y, z**y)
    assert sympify(l) == l2
    assert sympify(t) == t2
    assert sympify(l) != t2
    assert sympify(t) != l2

    assert l == l2
    assert t == t2
    assert l != t2
    assert t != l2

def test_exp():
    x = Symbol("x")
    e1 = sympy.exp(sympy.Symbol("x"))
    e2 = exp(x)
    assert sympify(e1) == e2
    assert e1 == e2._sympy_()

    e1 = sympy.exp(sympy.Symbol("x")).diff(sympy.Symbol("x"))
    e2 = exp(x).diff(x)
    assert sympify(e1) == e2
    assert e1 == e2._sympy_()

def test_constants():
    assert sympify(sympy.E) == E
    assert sympy.E == E._sympy_()

    assert sympify(sympy.pi) == pi
    assert sympy.pi == pi._sympy_()

def test_abs():
    x = Symbol("x")
    e1 = abs(sympy.Symbol("x"))
    e2 = abs(x)
    assert sympify(e1) == e2
    assert e1 == e2._sympy_()

    e1 = abs(2*sympy.Symbol("x"))
    e2 = 2*abs(x)
    assert sympify(e1) == e2
    assert e1 == e2._sympy_()

    y = Symbol("y")
    e1 = abs(sympy.Symbol("y")*sympy.Symbol("x"))
    e2 = abs(y*x)
    assert sympify(e1) == e2
    assert e1 == e2._sympy_()

def test_mpfr():
    if have_mpfr:
        a = RealMPFR('100', 100)
        b = sympy.Float('100', 29)
        assert sympify(b) == a
        assert b == a._sympy_()

def test_mpc():
    if have_mpc:
        a = ComplexMPC('1', '2', 100)
        b = sympy.Float(1, 29) + sympy.Float(2, 29) * sympy.I
        assert sympify(b) == a
        assert b == a._sympy_()

def test_log():
    x = Symbol("x")
    x1 = sympy.Symbol("x")

    assert log(x) == log(x1)
    assert log(x)._sympy_() == sympy.log(x1)
    assert sympify(sympy.log(x1)) == log(x)

    y = Symbol("y")
    y1 = sympy.Symbol("y")

    assert log(x, y) == log(x, y1)
    assert log(x1, y) == log(x1, y1)
    assert log(x, y)._sympy_() == sympy.log(x1, y1)
    assert sympify(sympy.log(x1, y1)) == log(x, y)
