from nose.tools import raises

from csympy import Symbol, Integer, sympify, SympifyError

def test_sympify1():
    assert sympify(1) == Integer(1)
    assert sympify(2) != Integer(1)
    assert sympify(-5) == Integer(-5)
    assert sympify(Integer(3)) == Integer(3)

@raises(SympifyError)
def test_sympify_error1a():
    e = sympify("if")

def test_sympify_error1b():
    assert not sympify("if", raise_error=False)

@raises(SympifyError)
def test_error1():
    e = sympify("x")

@raises(SympifyError)
def test_error2():
    e = sympify("   x")

@raises(SympifyError)
def test_error3():
    e = sympify("   x   ")
