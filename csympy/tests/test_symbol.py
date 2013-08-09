from csympy import Symbol, var

def test_symbol():
    x = Symbol("x")
    assert str(x) == "x"
    assert str(x) != "y"
    assert repr(x) == str(x)

def test_var():
    var("z")
    assert z == Symbol("z")
