from nose.tools import raises

from csympy import Integer

def test_integer():
    i = Integer(5)
    assert str(i) == "5"

def test_integer_long():
    i = Integer(123434444444444444444)
    assert str(i) == "123434444444444444444"

def test_integer_string():
    i = Integer("133")
    assert str(i) == "133"

@raises(ValueError)
def test_integer_invalid():
    i = Integer("133x")
