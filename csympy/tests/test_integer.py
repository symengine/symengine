from csympy import Rational

def test_integer():
    i = Rational(5)
    assert str(i) == "5"
