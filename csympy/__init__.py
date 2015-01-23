from .lib.csympy_wrapper import (Symbol, Integer, sympify, SympifyError, Add,
        Mul, Pow, exp, sin, cos, sqrt, function_symbol, I, E, pi)
from .utilities import var, symbols

def test():
    import pytest, os
    return not pytest.cmdline.main(
        [os.path.dirname(os.path.abspath(__file__))])
