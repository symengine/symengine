from csympy import (Symbol, Integer, sympify, SympifyError, sin, cos,
        function_symbol, I)
from csympy.lib.csympy_wrapper import densematrix
import sympy

# Note: We test _sympy_() for CSymPy -> SymPy conversion, as those are methods
# that are implemented in this library. Users can simply use sympy.sympify(...)
# to do this conversion, as this function will call our _sympy_() methods under
# the hood.
#
# For SymPy -> CSymPy, we test csympy.sympify(...) which does the conversion.

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
    assert sin(x/3)._sympy_() == sympy.sin(sympy.Symbol("x") / 3)
    assert sin(x/3)._sympy_() != sympy.cos(sympy.Symbol("x") / 3)
    assert cos(x/3)._sympy_() == sympy.cos(sympy.Symbol("x") / 3)

def test_conv7b():
    x = sympy.Symbol("x")
    y = sympy.Symbol("y")
    assert sympify(sympy.sin(x/3)) == sin(Symbol("x") / 3)
    assert sympify(sympy.sin(x/3)) != cos(Symbol("x") / 3)
    assert sympify(sympy.cos(x/3)) == cos(Symbol("x") / 3)

def test_conv8():
    e1 = sympy.Function("f")(sympy.Symbol("x"))
    e2 = function_symbol("f", Symbol("x"))
    assert e2._sympy_() == e1
    assert sympify(e1) == e2

    e3 = sympy.Function("q")(sympy.Symbol("t"))
    e4 = function_symbol("q", Symbol("t"))
    assert e4._sympy_() == e3
    assert e4._sympy_() != e1
    assert sympify(e3) == e4
    assert sympify(e3) != e2

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
    A = densematrix(1, 4, [Integer(1), Integer(2), Integer(3), Integer(4)])
    assert A._sympy_() == sympy.Matrix(1, 4, [sympy.Integer(1), sympy.Integer(2),
        sympy.Integer(3), sympy.Integer(4)])

    B = densematrix(4, 1, [Symbol("x"), Symbol("y"), Symbol("z"), Symbol("t")])
    assert B._sympy_() == sympy.Matrix(4, 1, [sympy.Symbol("x"), sympy.Symbol("y"),
        sympy.Symbol("z"), sympy.Symbol("t")])

    C = densematrix(2, 2,
        [Integer(5), Symbol("x"), function_symbol("f", Symbol("x")), 1 + I])

    assert C._sympy_() == sympy.Matrix([[5, sympy.Symbol("x")],
        [sympy.Function("f")(sympy.Symbol("x")), 1 + sympy.I]])

def test_conv10b():
    A = sympy.Matrix([[sympy.Symbol("x"), sympy.Symbol("y")],
        [sympy.Symbol("z"), sympy.Symbol("t")]])
    assert sympify(A) == densematrix(2, 2, [Symbol("x"), Symbol("y"),
        Symbol("z"), Symbol("t")])

    B = sympy.Matrix([[1, 2], [3, 4]])
    assert sympify(B) == densematrix(2, 2, [Integer(1), Integer(2), Integer(3),
        Integer(4)])

    C = sympy.Matrix([[7, sympy.Symbol("y")],
        [sympy.Function("g")(sympy.Symbol("z")), 3 + 2*sympy.I]])
    assert sympify(C) == densematrix(2, 2, [Integer(7), Symbol("y"),
        function_symbol("g", Symbol("z")), 3 + 2*I])
