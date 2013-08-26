from csympy import Symbol, sin, cos, sqrt, Add, function_symbol

def test_sin():
    x = Symbol("x")
    y = Symbol("y")
    e = sin(x)
    assert e.subs({x: y}) == sin(y)
    assert e.subs({x: y}) != sin(x)
