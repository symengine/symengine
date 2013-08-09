from csympy import Symbol, var

def test_symbol():
    x = Symbol("x")
    assert str(x) == "x"
    assert str(x) != "y"

def test_var():
    var("z")
    assert z == Symbol("z")
