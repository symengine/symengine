from csympy import Symbol, Integer, sympify, SympifyError
import sympy

def test_conv1():
    x = Symbol("x")
    assert sympy.sympify(x) == sympy.Symbol("x")
