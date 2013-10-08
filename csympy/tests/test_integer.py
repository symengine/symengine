from nose.tools import raises

from csympy import Integer

def test_integer():
    i = Integer(5)
    assert str(i) == "5"

def test_integer_long():
    i = Integer(123434444444444444444)
    assert str(i) == "123434444444444444444"

@raises(TypeError)
def test_integer_string():
    i = Integer("133")

def test_smallfloat_valid():
    i = Integer(7.5)
    assert str(i) == "7"

@raises(ValueError)
def test_bigfloat_invalid():
    i = Integer(13333333333333333.5)
