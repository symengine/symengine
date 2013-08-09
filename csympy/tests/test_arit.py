from nose.tools import raises

from csympy import Symbol, Integer

def test_arit1():
    x = Symbol("x")
    y = Symbol("y")
    e = x + y
    e = x * y
    e = Integer(2)*x
    e = 2*x

def test_arit2():
    x = Symbol("x")
    y = Symbol("y")
    assert x+x == Integer(2) * x
    assert x+x != Integer(3) * x
    assert x+y == y+x
    assert x+x == 2*x
    assert x+x == x*2
    assert x+x+x == 3*x
    assert x+y+x+x == 3*x+y

@raises(TypeError)
def test_arit3():
    x = Symbol("x")
    y = Symbol("y")
    e = "x"*x

def test_arit4():
    x = Symbol("x")
    y = Symbol("y")
    assert x*x == x**2
    assert x*y == y*x
    assert x*x*x == x**3
    assert x*y*x*x == x**3*y
