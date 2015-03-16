from csympy.utilities import raises
from csympy import Symbol, sin, cos
from csympy.lib.csympy_wrapper import eval_double

def test_eval_double1():
    x = Symbol("x")
    y = Symbol("y")
    e = sin(x)**2 + cos(x)**2
    e = e.subs(x, 7)
    assert abs(eval_double(e) - 1) < 1e-9

def test_eval_double2():
    x = Symbol("x")
    y = Symbol("y")
    e = sin(x)**2 + cos(x)**2
    raises(RuntimeError, lambda: (abs(eval_double(e) - 1) < 1e-9))
