from .lib.symengine_wrapper import (Symbol, Integer, sympify, SympifyError,
        Add, Mul, Pow, exp, log, sqrt, function_symbol, I, E, pi,
        have_mpfr, have_mpc, RealDouble, ComplexDouble, DenseMatrix,
        sin, cos, tan, cot, csc, sec, asin, acos, atan, acot, acsc, asec,
        sinh, cosh, tanh, coth, asinh, acosh, atanh, acoth)
from .utilities import var, symbols

if have_mpfr:
    from .lib.symengine_wrapper import RealMPFR

if have_mpc:
    from .lib.symengine_wrapper import ComplexMPC

def test():
    import pytest, os
    return not pytest.cmdline.main(
        [os.path.dirname(os.path.abspath(__file__))])
