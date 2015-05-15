from csympy.utilities import raises

from csympy import Symbol, Integer, Add, Pow

def test_arit1():
    x = Symbol("x")
    y = Symbol("y")
    e = x + y
    e = x * y
    e = Integer(2)*x
    e = 2*x
    e = x + 1
    e = 1 + x

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

    assert not x+x == 3*x
    assert not x+x != 2*x

def test_arit3():
    x = Symbol("x")
    y = Symbol("y")
    raises(TypeError, lambda: ("x"*x))

def test_arit4():
    x = Symbol("x")
    y = Symbol("y")
    assert x*x == x**2
    assert x*y == y*x
    assert x*x*x == x**3
    assert x*y*x*x == x**3*y

def test_arit5():
    x = Symbol("x")
    y = Symbol("y")
    e = (x+y)**2
    f = e.expand()
    assert e == (x+y)**2
    assert e != x**2 + 2*x*y + y**2
    assert isinstance(e, Pow)
    assert f == x**2 + 2*x*y + y**2
    assert isinstance(f, Add)

def test_arit6():
    x = Symbol("x")
    y = Symbol("y")
    e = x + y
    assert str(e) == "x + y" or "y + x"
    e = x * y
    assert str(e) == "x*y" or "y*x"
    e = Integer(2)*x
    assert str(e) == "2*x"
    e = 2*x
    assert str(e) == "2*x"

def test_arit7():
    x = Symbol("x")
    y = Symbol("y")
    assert x - x == 0
    assert x - y != y - x
    assert 2*x - x == x
    assert 3*x - x == 2*x

    assert 2*x*y - x*y == x*y

def test_arit8():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    assert x**y * x**x == x**(x+y)
    assert x**y * x**x * x**z == x**(x+y+z)
    assert x**y - x**y == 0

    assert x**2 / x == x
    assert y*x**2 / (x*y) == x
    assert (2 * x**3 * y**2 * z)**3 / 8 == x**9 * y**6 * z**3
    assert (2*y**(-2*x**2)) * (3*y**(2*x**2)) == 6

def test_expand1():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    assert ((2*x+y)**2).expand() == 4*x**2 + 4*x*y + y**2
    assert (x**2)**3 == x**6
    assert ((2*x**2+3*y)**2).expand() == 4*x**4 + 12*x**2*y + 9*y**2
    assert ((2*x/3+y/4)**2).expand() == 4*x**2/9 + x*y/3 + y**2/16

def test_arit9():
    x = Symbol("x")
    y = Symbol("y")
    assert 1/x == 1/x
    assert 1/x != 1/y

def test_expand2():
    y = Symbol("y")
    z = Symbol("z")
    assert ((1/(y*z) - y*z)*y*z).expand() == 1-(y*z)**2

def test_expand3():
    x = Symbol("x")
    y = Symbol("y")
    assert ((1/(x*y) - x*y+2)*(1+x*y)).expand() == 3 + 1/(x*y) + x*y - (x*y)**2

def test_args():
    x = Symbol("x")
    y = Symbol("y")
    assert (x**2).args == (x, 2)
    assert (x**2 + 5).args == (5, x**2)
    assert set((x**2 + 2*x*y + 5).args) == set((x**2, 2*x*y, 5))
    assert (2*x**2).args == (2, x**2)
    assert set((2*x**2*y).args) == set((2, x**2, y))

def test_free_symbols():
    x = Symbol("x")
    y = Symbol("y")
    z = Symbol("z")
    assert (x**2).free_symbols == set([x])
    assert (x**y + z).free_symbols == set([x, y, z])
