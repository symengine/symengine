from csympy import Symbol, sin, cos, sqrt, Add

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
