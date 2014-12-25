from csympy import Symbol, sin, cos, sqrt, Add, Mul, function_symbol, Integer
from csympy.lib.csympy_wrapper import Subs, Derivative

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
    z = Symbol("z")
    f = function_symbol("f", x)
    g = function_symbol("g", x)
    assert f != g

    f = function_symbol("f", x)
    g = function_symbol("f", x)
    assert f == g

    f = function_symbol("f", x, y)
    g = function_symbol("f", y, x)
    assert f != g

    f = function_symbol("f", x, y)
    g = function_symbol("f", x, y)
    assert f == g

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

    f = function_symbol("f", x, y)
    assert f.diff(x).diff(y) == function_symbol("f", x, y).diff(x).diff(y)
    assert f.diff(Symbol("z")) == 0

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
    y = Symbol("y")
    e = abs(x)
    assert e.diff(x) != e
    assert e.diff(x) != 0
    assert e.diff(y) == 0

def test_Subs():
    x = Symbol("x")
    y = Symbol("y")
    _x = Symbol("_x")
    f = function_symbol("f", 2*x)
    assert f.diff(x) == 2 * Subs(Derivative(function_symbol("f", _x), [_x]), [_x], [2 * x])
    assert Subs(Derivative(function_symbol("f", x, y), [x]), [x, y], [_x, x]) \
                == Subs(Derivative(function_symbol("f", x, y), [x]), [y, x], [x, _x])

def test_FunctionWrapper():
    import sympy
    sympy.var("n, m, theta, phi")
    r = sympy.Ynm(n, m, theta, phi)
    s = Integer(2)*r
    assert isinstance(s, Mul)
    assert isinstance(s.args[1]._sympy_(), sympy.Ynm)
