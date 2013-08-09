from nose.tools import raises

from csympy import Symbol, Integer, sympify, SympifyError

def test_sympify1():
    assert sympify(1) == Integer(1)
    assert sympify(2) != Integer(1)

@raises(SympifyError)
def test_sympify_error1a():
    e = sympify("if")

def test_sympify_error1b():
    assert not sympify("if", raise_error=False)
