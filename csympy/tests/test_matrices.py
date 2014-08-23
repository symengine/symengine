from nose.tools import raises
from csympy.lib.csympy_wrapper import densematrix

def test_get():
	x = densematrix(2, 2, [1, 2, 3, 4])
	assert x.get(0, 0) == 1
	assert x.get(0, 1) == 2
	assert x.get(1, 1) == 4
