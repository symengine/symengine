from csympy import (Symbol, Integer, sympify, SympifyError, sin, cos,
        function_symbol)
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
