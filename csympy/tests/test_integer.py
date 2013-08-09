from csympy import Integer

def test_integer():
    i = Integer(5)
    assert str(i) == "5"
