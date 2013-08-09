from csympy import Symbol

def test_symbol():
    x = Symbol("x")
    assert str(x) == "x"
    assert str(x) != "y"
