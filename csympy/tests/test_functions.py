from csympy import Symbol, sin, cos, sqrt, Add, function_symbol, Integer

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
    assert f.diff(x).args == (f, x)
    assert f.diff(x).diff(x).args == (f, x, x)

    g = function_symbol("f", y)
    assert g.diff(x) == 0
    assert g.diff(y) == function_symbol("f", y).diff(y)
    assert g.diff(y).diff(y) == function_symbol("f", y).diff(y).diff(y)

    assert f - function_symbol("f", x) == 0

def test_abs():
    x = Symbol("x")
    e = abs(x)
    assert e == abs(x)
    assert e != cos(x)

    assert abs(5) == 5
    assert abs(-5) == 5
    assert abs(Integer(5)/3) == Integer(5)/3
    assert abs(-Integer(5)/3) == Integer(5)/3
    assert abs(Integer(5)/3+x) != Integer(5)/3
    assert abs(Integer(5)/3+x) == abs(Integer(5)/3+x)

def test_abs_diff():
    x = Symbol("x")
    e = abs(x)
    assert e.diff(x) != e
