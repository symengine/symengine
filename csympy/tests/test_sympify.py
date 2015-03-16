from csympy.utilities import raises

from csympy import Symbol, Integer, sympify, SympifyError

def test_sympify1():
    assert sympify(1) == Integer(1)
    assert sympify(2) != Integer(1)
    assert sympify(-5) == Integer(-5)
    assert sympify(Integer(3)) == Integer(3)

def test_sympify_error1a():
    raises(SympifyError, lambda: sympify("if"))

def test_sympify_error1b():
    assert not sympify("if", raise_error=False)

def test_error1():
    raises(SympifyError, lambda: sympify("x"))

def test_error2():
    raises(SympifyError, lambda: sympify("   x"))

def test_error3():
    raises(SympifyError, lambda: sympify("   x   "))
