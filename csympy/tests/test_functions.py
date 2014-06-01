from csympy import Symbol, sin, cos, sqrt, Add, function_symbol

def test_sin():
    x = Symbol("x")
    e = sin(x)
    assert e == sin(x)
    assert e != cos(x)

    assert sin(x).diff(x) == cos(x)
    assert cos(x).diff(x) == -sin(x)

    e = sqrt(x).diff(x).diff(x)
    f = sin(e)
    g = f.diff(x).diff(x)
    assert isinstance(g, Add)

def test_f():
    x = Symbol("x")
    y = Symbol("y")
    f = function_symbol("f", x)
    g = function_symbol("g", x)
    assert f != g

    f = function_symbol("f", x)
    g = function_symbol("f", x)
    assert f == g

    f = function_symbol("f", x)
    assert f.diff(y) == 0

def test_derivative():
    x = Symbol("x")
    y = Symbol("y")
    f = function_symbol("f", x)
    assert f.diff(x) == function_symbol("f", x).diff(x)
    assert f.diff(x).diff(x) == function_symbol("f", x).diff(x).diff(x)
    assert f.diff(y) == 0

    g = function_symbol("f", y)
    assert g.diff(x) == 0
    assert g.diff(y) == function_symbol("f", y).diff(y)
    assert g.diff(y).diff(y) == function_symbol("f", y).diff(y).diff(y)

    assert f - function_symbol("f", x) == 0
