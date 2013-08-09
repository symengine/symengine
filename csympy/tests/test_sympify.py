from nose.tools import raises

from csympy import Symbol, Integer, sympify

def test_sympify1():
    assert sympify(1) == Integer(1)
    assert sympify(2) != Integer(1)
