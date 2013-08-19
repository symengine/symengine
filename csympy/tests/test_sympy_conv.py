from csympy import Symbol, Integer, sympify, SympifyError
import sympy

def test_conv1():
    x = Symbol("x")
    assert x._sympy_() == sympy.Symbol("x")
    assert x._sympy_() != sympy.Symbol("y")
    x = Symbol("y")
    assert x._sympy_() != sympy.Symbol("x")
    assert x._sympy_() == sympy.Symbol("y")

def test_conv2():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    e = x*y
    assert e._sympy_() == sympy.Symbol("x")*sympy.Symbol("y")
    e = x*y*z
    assert e._sympy_() == sympy.Symbol("x")*sympy.Symbol("y")*sympy.Symbol("z")

def test_conv3():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    e = x+y
    assert sympy.sympify(e) == sympy.Symbol("x")+sympy.Symbol("y")
    e = x+y+z
    assert sympy.sympify(e) == sympy.Symbol("x")+sympy.Symbol("y")+sympy.Symbol("z")

def test_conv4():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    e = x**y
    assert e._sympy_() == sympy.Symbol("x")**sympy.Symbol("y")
    e = (x+y)**z
    assert sympy.sympify(e) == (sympy.Symbol("x")+sympy.Symbol("y"))**sympy.Symbol("z")

def test_conv5():
    x = Integer(5)
    y = Integer(6)
    assert sympy.sympify(x) == sympy.Integer(5)
    assert sympy.sympify(x/y) == sympy.Integer(5) / sympy.Integer(6)

def test_conv6():
    x = Symbol("x")
    y = Symbol("y")
    assert (x/3)._sympy_() == sympy.Symbol("x") / 3
    #assert sympy.sympify(3*x) == 3*sympy.Symbol("x")
    assert sympy.sympify(3+x) == 3+sympy.Symbol("x")
    #assert sympy.sympify(3-x) == 3-sympy.Symbol("x")
    assert sympy.sympify(x/y) == sympy.Symbol("x") / sympy.Symbol("y")
